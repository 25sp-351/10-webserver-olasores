#ifndef ROUTE_HANDLERS_H
#define ROUTE_HANDLERS_H

#include "request.h"
#include "response.h"

/**
 * Handle a request to the /static/ path
 * @param request The HTTP request
 * @param response The HTTP response to fill
 */
void handle_static_request(const http_request_t* request,
                           http_response_t* response);

/**
 * Handle a request to the /calc/ path
 * @param request The HTTP request
 * @param response The HTTP response to fill
 */
void handle_calc_request(const http_request_t* request,
                         http_response_t* response);

/**
 * Handle a request to the /sleep/ path (for pipelining test)
 * @param request The HTTP request
 * @param response The HTTP response to fill
 */
void handle_sleep_request(const http_request_t* request,
                          http_response_t* response);
#endif /* ROUTE_HANDLERS_H */
