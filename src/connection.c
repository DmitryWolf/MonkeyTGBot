#include "connection.h"

#include <openssl/err.h>
#include <unistd.h>
#include <netdb.h>

int connection_init(connection* context, const char *host, const char *port, int type) {
    if (type == 0) {
        context->ctx = init_ssl_context();
    }
    if (type == 1) {
        SSL_shutdown(context->ssl);
        close(context->sockfd);
        SSL_free(context->ssl);
    }
    context->sockfd = create_socket_connection(host, port); // TODO: check is_socket_alive
    if (context->sockfd == -1) {
        SSL_CTX_free(context->ctx);
        perror("create socket connection");
        return -1;
    }
    if (make_SSL_connection(context) == -1) {
        perror("create SSL connection");
        return -1;
    }
    return 0;
}

void connection_destroy(connection* context) {
    SSL_shutdown(context->ssl);
    close(context->sockfd);
    SSL_free(context->ssl);
    SSL_CTX_free(context->ctx);
}

SSL_CTX *init_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_ssl_algorithms();
    method = TLS_client_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

int make_SSL_connection(connection* context) {
    context->ssl = SSL_new(context->ctx);
    SSL_set_fd(context->ssl, context->sockfd);
    if (SSL_connect(context->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(context->sockfd);
        SSL_free(context->ssl);
        SSL_CTX_free(context->ctx);
        perror("SSL connect");
        return -1;
    }
    return 0;
}


int create_socket_connection(const char *host, const char *port) {
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return sockfd;
}