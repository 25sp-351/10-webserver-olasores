#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

void print_usage(const char* program_name) {
    printf("Usage: %s [-p port]\n", program_name);
    printf("  -p port    Port to listen on (default: 80)\n");
}

int main(int argc, char* argv[]) {
    int port = 80;
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                if (port <= 0 || port > 65535) {
                    fprintf(stderr, "Invalid port number\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    printf("Starting server on port %d\n", port);

    if (start_server(port) != 0) {
        fprintf(stderr, "Failed to start server\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
