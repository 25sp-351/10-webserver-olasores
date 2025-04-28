#include "request.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_request(const char* buffer, http_request_t* request) {
    if (!buffer || !request)
        return -1;

    memset(request, 0, sizeof(http_request_t));

    char* buffer_copy = strdup(buffer);
    if (!buffer_copy)
        return -1;

    char* line = strtok(buffer_copy, "\r\n");
    if (!line) {
        free(buffer_copy);
        return -1;
    }

    if (sscanf(line, "%15s %2047s %15s", request->method, request->path,
               request->http_version) != 3) {
        free(buffer_copy);
        return -1;
    }

    request->num_headers = 0;
    while ((line = strtok(NULL, "\r\n")) != NULL && *line) {
        if (request->num_headers >= MAX_HEADERS)
            break;

        char* colon = strchr(line, ':');
        if (!colon)
            continue;  // Invalid header line

        size_t name_len = colon - line;
        if (name_len >= MAX_HEADER_NAME_LENGTH)
            name_len = MAX_HEADER_NAME_LENGTH - 1;
        strncpy(request->headers[request->num_headers].name, line, name_len);
        request->headers[request->num_headers].name[name_len] = '\0';

        const char* value                                     = colon + 1;
        while (*value && isspace(*value))
            value++;

        strncpy(request->headers[request->num_headers].value, value,
                MAX_HEADER_VALUE_LENGTH - 1);
        request->headers[request->num_headers]
            .value[MAX_HEADER_VALUE_LENGTH - 1] = '\0';

        request->num_headers++;
    }

    free(buffer_copy);
    return 0;
}

const char* get_header_value(const http_request_t* request, const char* name) {
    if (!request || !name)
        return NULL;

    for (int i = 0; i < request->num_headers; i++)
        if (strcasecmp(request->headers[i].name, name) == 0)
            return request->headers[i].value;

    return NULL;
}
