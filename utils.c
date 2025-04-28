#include "utils.h"

#include <ctype.h>
#include <string.h>

const char* get_mime_type(const char* filename) {
    if (!filename)
        return "application/octet-stream";

    const char* ext = strrchr(filename, '.');
    if (!ext)
        return "application/octet-stream";

    ext++;

    char ext_lower[32] = {0};
    size_t i;
    for (i = 0; i < sizeof(ext_lower) - 1 && ext[i]; i++)
        ext_lower[i] = tolower(ext[i]);
    ext_lower[i] = '\0';

    if (strcmp(ext_lower, "html") == 0 || strcmp(ext_lower, "htm") == 0)
        return "text/html";
    else if (strcmp(ext_lower, "txt") == 0)
        return "text/plain";
    else if (strcmp(ext_lower, "css") == 0)
        return "text/css";
    else if (strcmp(ext_lower, "js") == 0)
        return "application/javascript";
    else if (strcmp(ext_lower, "json") == 0)
        return "application/json";
    else if (strcmp(ext_lower, "xml") == 0)
        return "application/xml";
    else if (strcmp(ext_lower, "jpg") == 0 || strcmp(ext_lower, "jpeg") == 0)
        return "image/jpeg";
    else if (strcmp(ext_lower, "png") == 0)
        return "image/png";
    else if (strcmp(ext_lower, "gif") == 0)
        return "image/gif";
    else if (strcmp(ext_lower, "svg") == 0)
        return "image/svg+xml";
    else if (strcmp(ext_lower, "ico") == 0)
        return "image/x-icon";
    else if (strcmp(ext_lower, "pdf") == 0)
        return "application/pdf";
    else if (strcmp(ext_lower, "zip") == 0)
        return "application/zip";
    else if (strcmp(ext_lower, "mp3") == 0)
        return "audio/mpeg";
    else if (strcmp(ext_lower, "mp4") == 0)
        return "video/mp4";
    else if (strcmp(ext_lower, "webm") == 0)
        return "video/webm";
    else if (strcmp(ext_lower, "woff") == 0)
        return "font/woff";
    else if (strcmp(ext_lower, "woff2") == 0)
        return "font/woff2";
    else if (strcmp(ext_lower, "ttf") == 0)
        return "font/ttf";
    else if (strcmp(ext_lower, "otf") == 0)
        return "font/otf";

    return "application/octet-stream";
}
