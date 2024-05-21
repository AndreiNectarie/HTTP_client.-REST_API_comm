#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <iostream>
#include "helpers.h"
#include "requests.h"

using namespace std;

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token)
{
    int i;
    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    sprintf(line, "User-Agent: Mozilla/5.0");
    compute_message(message, line);

    sprintf(line, "Connection: keep-alive");
    compute_message(message, line);

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies[0]);
        compute_message(message, line);
    }
    // step 4: add Authorization header
    if (token != nullptr) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");

    free(line);

    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char* token)
{
    if (!host || !url || !content_type || !body_data) {
        return NULL; // return if any of the pointers are null
    }

    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    if (!message || !line) {
        return NULL; // return if memory allocation failed
    }

    // Step 1: write the method name, URL and protocol

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    int content_length = strlen(body_data[0]);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    sprintf(line, "Connection: keep-alive");
    compute_message(message, line);

    // Step 4: add cookies if any
    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies[0]);
        compute_message(message, line);
    }

    // step 5: add Authorization header
    if (token != nullptr) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    // Step 6: add new line at end of headers
    compute_message(message, "");

    // Step 7: add the actual payload data
    sprintf(line, "%s", body_data[0]);
    compute_message(message, line);

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char **cookies, int cookies_count, char* token)
{
    if (!host || !url) {
        return NULL; // return if any of the pointers are null
    }

    char *message = (char*)calloc(BUFLEN, sizeof(char));
    char *line = (char*)calloc(LINELEN, sizeof(char));

    if (!message || !line) {
        return NULL; // return if memory allocation failed
    }

    // Step 1: write the method name, URL and protocol
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Connection: keep-alive");
    compute_message(message, line);

    // Step 3: add cookies if any
    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies[0]);
        compute_message(message, line);
    }

    // Step 4: add Authorization header
    if (token != nullptr) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 5: add new line at end of headers
    compute_message(message, "");

    free(line);
    return message;
}