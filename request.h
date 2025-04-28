#ifndef REQUEST_H
#define REQUEST_H

#define MAX_PATH_LENGTH 2048
#define MAX_HEADERS 50
#define MAX_HEADER_NAME_LENGTH 128
#define MAX_HEADER_VALUE_LENGTH 1024

typedef struct {
    char name[MAX_HEADER_NAME_LENGTH];
    char value[MAX_HEADER_VALUE_LENGTH];
} http_header_t;

typedef struct {
    char method[16];
    char path[MAX_PATH_LENGTH];
    char http_version[16];
    http_header_t headers[MAX_HEADERS];
    int num_headers;
} http_request_t;

/**
 * Parse an HTTP request from a buffer
 * @param buffer Buffer containing the HTTP request
 * @param request Pointer to an http_request_t structure to fill
 * @return 0 on success, non-zero on error
 */
int parse_request(const char* buffer, http_request_t* request);

/**
 * Find a header value in the request
 * @param request The HTTP request
 * @param name The header name to find
 * @return The header value, or NULL if not found
 */
const char* get_header_value(const http_request_t* request, const char* name);

#endif /* REQUEST_H */
