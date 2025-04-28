#include "server.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "request.h"
#include "response.h"
#include "route_handlers.h"

#define MAX_CONNECTIONS 100
#define BUFFER_SIZE 8192

// Structure to pass client information to thread
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} client_info_t;

// Thread function to handle a client connection
void* handle_client(void* arg) {
    client_info_t* client_info     = (client_info_t*)arg;
    int client_fd                  = client_info->client_fd;
    struct sockaddr_in client_addr = client_info->client_addr;

    // Free the client_info structure as we've extracted what we need
    free(client_info);

    // Get client IP address as string
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    // Buffer for receiving data
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Read the request
    bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        // Null-terminate the received data
        buffer[bytes_received] = '\0';

        // Parse the request
        http_request_t request;
        if (parse_request(buffer, &request) == 0) {
            // Create a response
            http_response_t response;
            init_response(&response);

            // Route the request to the appropriate handler
            if (strncmp(request.path, "/static/", 8) == 0) {
                handle_static_request(&request, &response);
            } else if (strncmp(request.path, "/calc/", 6) == 0) {
                handle_calc_request(&request, &response);
            } else if (strncmp(request.path, "/sleep/", 7) == 0) {
                handle_sleep_request(&request, &response);
            } else {
                // Handle 404 Not Found
                set_response_status(&response, 404, "Not Found");
                set_response_content_type(&response, "text/plain");
                set_response_content(&response, "404 Not Found", 13);
            }

            // Send the response
            char response_buffer[BUFFER_SIZE];
            int response_size =
                format_response(&response, response_buffer, BUFFER_SIZE);
            send(client_fd, response_buffer, response_size, 0);

            // Free response resources
            free_response(&response);
        } else {
            // Bad request
            http_response_t response;
            init_response(&response);
            set_response_status(&response, 400, "Bad Request");
            set_response_content_type(&response, "text/plain");
            set_response_content(&response, "400 Bad Request", 15);

            char response_buffer[BUFFER_SIZE];
            int response_size =
                format_response(&response, response_buffer, BUFFER_SIZE);
            send(client_fd, response_buffer, response_size, 0);

            free_response(&response);
        }
    }

    // Close the connection
    close(client_fd);
    printf("Connection closed with %s:%d\n", client_ip,
           ntohs(client_addr.sin_port));

    return NULL;
}

int start_server(int port) {
    // Ignore SIGPIPE signal (happens when client disconnects)
    signal(SIGPIPE, SIG_IGN);

    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Failed to set socket options");
        close(server_fd);
        return 1;
    }

    // Prepare the server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
        0) {
        perror("Failed to bind socket");
        close(server_fd);
        return 1;
    }

    // Start listening
    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("Failed to listen");
        close(server_fd);
        return 1;
    }

    printf("Server listening on port %d\n", port);

    // Accept connections and create threads to handle them
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd =
            accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("Failed to accept connection");
            continue;
        }

        // Create a structure to pass to the thread
        client_info_t* client_info = malloc(sizeof(client_info_t));
        if (!client_info) {
            perror("Failed to allocate memory for client info");
            close(client_fd);
            continue;
        }

        client_info->client_fd   = client_fd;
        client_info->client_addr = client_addr;

        // Create a thread to handle the connection
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_info) != 0) {
            perror("Failed to create thread");
            free(client_info);
            close(client_fd);
            continue;
        }

        // Detach the thread so resources are automatically released when it
        // exits
        pthread_detach(thread_id);
    }

    // We'll never get here, but it's good practice
    close(server_fd);
    return 0;
}
