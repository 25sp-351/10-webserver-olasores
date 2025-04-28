#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INITIAL_HEADER_CAPACITY 10

void init_response(http_response_t* response) {
    if (!response)
        return;

    memset(response, 0, sizeof(http_response_t));

    response->status_code    = 200;
    response->status_text    = "OK";
    response->content_type   = "text/plain";
    response->content        = NULL;
    response->content_length = 0;

    response->max_headers    = INITIAL_HEADER_CAPACITY;
    response->header_names   = calloc(response->max_headers, sizeof(char*));
    response->header_values  = calloc(response->max_headers, sizeof(char*));
    response->num_headers    = 0;

    add_response_header(response, "Server", "SimpleHTTP/1.0");

    time_t now = time(NULL);
    struct tm tm_info;
    gmtime_r(&now, &tm_info);
    char date_str[64];
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", &tm_info);
    add_response_header(response, "Date", date_str);

    add_response_header(response, "Connection", "close");
}

void free_response(http_response_t* response) {
    if (!response)
        return;

    if (response->content) {
        free(response->content);
        response->content = NULL;
    }

    if (response->header_names) {
        for (int i = 0; i < response->num_headers; i++)
            if (response->header_names[i])
                free(response->header_names[i]);
        free(response->header_names);
        response->header_names = NULL;
    }

    if (response->header_values) {
        for (int i = 0; i < response->num_headers; i++)
            if (response->header_values[i])
                free(response->header_values[i]);
        free(response->header_values);
        response->header_values = NULL;
    }

    response->num_headers = 0;
    response->max_headers = 0;
}

void set_response_status(http_response_t* response, int code,
                         const char* text) {
    if (!response || !text)
        return;

    response->status_code = code;
    response->status_text = text;
}

void set_response_content_type(http_response_t* response,
                               const char* content_type) {
    if (!response || !content_type)
        return;

    response->content_type = content_type;
}

void set_response_content(http_response_t* response, const void* content,
                          size_t length) {
    if (!response)
        return;

    if (response->content)
        free(response->content);

    if (content && length > 0) {
        response->content = malloc(length);
        if (response->content) {
            memcpy(response->content, content, length);
            response->content_length = length;
        } else {
            response->content        = NULL;
            response->content_length = 0;
        }
    } else {
        response->content        = NULL;
        response->content_length = 0;
    }
}

int add_response_header(http_response_t* response, const char* name,
                        const char* value) {
    if (!response || !name || !value)
        return -1;

    if (response->num_headers >= response->max_headers) {
        int new_size = response->max_headers * 2;
        char** new_names =
            realloc(response->header_names, new_size * sizeof(char*));
        char** new_values =
            realloc(response->header_values, new_size * sizeof(char*));

        if (!new_names || !new_values) {
            if (new_names)
                free(new_names);
            if (new_values)
                free(new_values);
            return -1;
        }

        response->header_names  = new_names;
        response->header_values = new_values;
        response->max_headers   = new_size;
    }

    for (int i = 0; i < response->num_headers; i++) {
        if (strcasecmp(response->header_names[i], name) == 0) {
            free(response->header_values[i]);
            response->header_values[i] = strdup(value);
            return 0;
        }
    }

    response->header_names[response->num_headers]  = strdup(name);
    response->header_values[response->num_headers] = strdup(value);
    response->num_headers++;

    return 0;
}

int format_response(const http_response_t* response, char* buffer,
                    size_t buffer_size) {
    if (!response || !buffer || buffer_size == 0)
        return -1;

    int offset             = snprintf(buffer, buffer_size, "HTTP/1.1 %d %s\r\n",
                                      response->status_code, response->status_text);

    int content_type_found = 0;
    for (int i = 0; i < response->num_headers; i++) {
        if (strcasecmp(response->header_names[i], "Content-Type") == 0) {
            content_type_found = 1;
            break;
        }
    }

    if (!content_type_found && response->content_type) {
        offset += snprintf(buffer + offset, buffer_size - offset,
                           "Content-Type: %s\r\n", response->content_type);
    }

    int content_length_found = 0;
    for (int i = 0; i < response->num_headers; i++) {
        if (strcasecmp(response->header_names[i], "Content-Length") == 0) {
            content_length_found = 1;
            break;
        }
    }

    if (!content_length_found) {
        offset += snprintf(buffer + offset, buffer_size - offset,
                           "Content-Length: %zu\r\n", response->content_length);
    }

    for (int i = 0; i < response->num_headers; i++) {
        offset +=
            snprintf(buffer + offset, buffer_size - offset, "%s: %s\r\n",
                     response->header_names[i], response->header_values[i]);
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "\r\n");

    if (offset + response->content_length > buffer_size)
        return -1;

    if (response->content && response->content_length > 0) {
        memcpy(buffer + offset, response->content, response->content_length);
        offset += response->content_length;
    }

    return offset;
}
