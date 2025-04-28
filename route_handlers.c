#include "route_handlers.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

void handle_static_request(const http_request_t* request,
                           http_response_t* response) {
    if (strcmp(request->method, "GET") != 0) {
        set_response_status(response, 405, "Method Not Allowed");
        add_response_header(response, "Allow", "GET");
        set_response_content_type(response, "text/plain");
        set_response_content(response, "Method Not Allowed", 18);
        return;
    }

    const char* file_path = request->path + 7;  // Skip "/static/"

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "static/%s", file_path);

    int fd = open(full_path, O_RDONLY);
    if (fd < 0) {
        set_response_status(response, 404, "Not Found");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "File not found";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        set_response_status(response, 500, "Internal Server Error");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Failed to get file information";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    char* file_content = malloc(st.st_size);
    if (!file_content) {
        close(fd);
        set_response_status(response, 500, "Internal Server Error");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Out of memory";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    ssize_t bytes_read = read(fd, file_content, st.st_size);
    close(fd);

    if (bytes_read != st.st_size) {
        free(file_content);
        set_response_status(response, 500, "Internal Server Error");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Failed to read file";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    const char* content_type = get_mime_type(full_path);
    set_response_content_type(response, content_type);

    set_response_content(response, file_content, st.st_size);
    free(file_content);

    set_response_status(response, 200, "OK");
}

void handle_calc_request(const http_request_t* request,
                         http_response_t* response) {
    if (strcmp(request->method, "GET") != 0) {
        set_response_status(response, 405, "Method Not Allowed");
        add_response_header(response, "Allow", "GET");
        set_response_content_type(response, "text/plain");
        set_response_content(response, "Method Not Allowed", 18);
        return;
    }

    char path_copy[MAX_PATH_LENGTH];
    strncpy(path_copy, request->path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    char* operation                  = path_copy + 6;

    char* num1_str                   = strchr(operation, '/');
    if (!num1_str) {
        set_response_status(response, 400, "Bad Request");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Invalid calculation format";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    *num1_str = '\0';
    num1_str++;

    char* num2_str = strchr(num1_str, '/');
    if (!num2_str) {
        set_response_status(response, 400, "Bad Request");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Invalid calculation format";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    *num2_str = '\0';
    num2_str++;

    double num1, num2, result;
    char* endptr;

    num1 = strtod(num1_str, &endptr);
    if (*endptr != '\0') {
        set_response_status(response, 400, "Bad Request");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Invalid number format";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    num2 = strtod(num2_str, &endptr);
    if (*endptr != '\0') {
        set_response_status(response, 400, "Bad Request");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Invalid number format";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    const char* op_name;
    if (strcmp(operation, "add") == 0) {
        result  = num1 + num2;
        op_name = "Addition";
    } else if (strcmp(operation, "mul") == 0) {
        result  = num1 * num2;
        op_name = "Multiplication";
    } else if (strcmp(operation, "div") == 0) {
        if (num2 == 0) {
            set_response_status(response, 400, "Bad Request");
            set_response_content_type(response, "text/plain");
            const char* error_msg = "Division by zero";
            set_response_content(response, error_msg, strlen(error_msg));
            return;
        }
        result  = num1 / num2;
        op_name = "Division";
    } else {
        set_response_status(response, 400, "Bad Request");
        set_response_content_type(response, "text/plain");
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg), "Unknown operation: %s",
                 operation);
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    // HTML response for the c
    char html[4096];
    int html_len = snprintf(html, sizeof(html),
                            "<!DOCTYPE html>\n"
                            "<html>\n"
                            "<head>\n"
                            "    <title>Calculation Result</title>\n"
                            "</head>\n"
                            "<body>\n"
                            "    <h1>%s Result</h1>\n"
                            "    <p>%g %s %g = %g</p>\n"
                            "</body>\n"
                            "</html>",
                            op_name, num1,
                            strcmp(operation, "add") == 0
                                ? "+"
                                : (strcmp(operation, "mul") == 0 ? "*" : "/"),
                            num2, result);

    set_response_status(response, 200, "OK");
    set_response_content_type(response, "text/html");
    set_response_content(response, html, html_len);
}

// Handle sleep request
void handle_sleep_request(const http_request_t* request,
                          http_response_t* response) {
    if (strcmp(request->method, "GET") != 0) {
        set_response_status(response, 405, "Method Not Allowed");
        add_response_header(response, "Allow", "GET");
        set_response_content_type(response, "text/plain");
        set_response_content(response, "Method Not Allowed", 18);
        return;
    }

    const char* seconds_str = request->path + 7;

    char* endptr;
    long seconds = strtol(seconds_str, &endptr, 10);

    if (*endptr != '\0' || seconds < 0 || seconds > 10) {
        set_response_status(response, 400, "Bad Request");
        set_response_content_type(response, "text/plain");
        const char* error_msg = "Invalid sleep duration (must be 0-10 seconds)";
        set_response_content(response, error_msg, strlen(error_msg));
        return;
    }

    sleep(seconds);

    char html[1024];
    int html_len = snprintf(html, sizeof(html),
                            "<!DOCTYPE html>\n"
                            "<html>\n"
                            "<head>\n"
                            "    <title>Sleep Result</title>\n"
                            "</head>\n"
                            "<body>\n"
                            "    <h1>Sleep Complete</h1>\n"
                            "    <p>Server slept for %ld seconds.</p>\n"
                            "</body>\n"
                            "</html>",
                            seconds);

    set_response_status(response, 200, "OK");
    set_response_content_type(response, "text/html");
    set_response_content(response, html, html_len);
}
