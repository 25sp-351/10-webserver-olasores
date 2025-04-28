#ifndef RESPONSE_H
#define RESPONSE_H

#include <stddef.h>

typedef struct {
    int status_code;
    const char* status_text;
    const char* content_type;
    char* content;
    size_t content_length;

    // Headers
    char** header_names;
    char** header_values;
    int num_headers;
    int max_headers;
} http_response_t;

/**
 * Initialize an HTTP response structure
 * @param response Pointer to the response structure to initialize
 */
void init_response(http_response_t* response);

/**
 * Free resources used by a response
 * @param response Pointer to the response structure
 */
void free_response(http_response_t* response);

/**
 * Set the response status code and text
 * @param response Pointer to the response structure
 * @param code HTTP status code
 * @param text HTTP status text
 */
void set_response_status(http_response_t* response, int code, const char* text);

/**
 * Set the response content type
 * @param response Pointer to the response structure
 * @param content_type MIME type of the content
 */
void set_response_content_type(http_response_t* response,
                               const char* content_type);

/**
 * Set the response content
 * @param response Pointer to the response structure
 * @param content Content data
 * @param length Length of the content in bytes
 */
void set_response_content(http_response_t* response, const void* content,
                          size_t length);

/**
 * Add a header to the response
 * @param response Pointer to the response structure
 * @param name Header name
 * @param value Header value
 * @return 0 on success, non-zero on error
 */
int add_response_header(http_response_t* response, const char* name,
                        const char* value);

/**
 * Format the response into a buffer for sending
 * @param response Pointer to the response structure
 * @param buffer Buffer to write the formatted response to
 * @param buffer_size Size of the buffer
 * @return Number of bytes written to the buffer, or -1 on error
 */
int format_response(const http_response_t* response, char* buffer,
                    size_t buffer_size);

#endif /* RESPONSE_H */
