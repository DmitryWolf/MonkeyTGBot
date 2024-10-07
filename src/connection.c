#include "connection.h"

#include <openssl/err.h>
#include <unistd.h>
#include <netdb.h>

void SSL_init() {
    SSL_library_init();
    // Need in old versions
    // SSL_load_error_strings();
    // OpenSSL_add_ssl_algorithms();
}

void SSL_destroy() {
    // Need in old versions
    // EVP_cleanup();
    // ERR_free_strings();
    // SSL_COMP_free_compression_methods();
}

int connection_init(connection* context, const char *host, const char *port) {
    context->ctx = init_ssl_context();
    if (!context->ctx) {
        return -1;
    }

    context->sockfd = create_socket_connection(host, port);
    if (context->sockfd == -1) {
        SSL_CTX_free(context->ctx);
        return -1;
    }

    if (make_SSL_connection(context) == -1) {
        close(context->sockfd);
        SSL_CTX_free(context->ctx);
        return -1;
    }
    return 0;
}

void connection_destroy(connection* context) {
    if (context->ssl) {
        SSL_shutdown(context->ssl);
        SSL_free(context->ssl);
    }

    if (context->sockfd != -1) {
        close(context->sockfd);
    }

    if (context->ctx) {
        SSL_CTX_free(context->ctx);
    }
}

int connection_restart(connection* context, const char *host, const char *port) {
    connection_destroy(context);
    return connection_init(context, host, port);
}

SSL_CTX *init_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return ctx;
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


int make_SSL_connection(connection* context) {
    context->ssl = SSL_new(context->ctx);
    if (!context->ssl) {
        perror("Unable to create SSL structure");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    SSL_set_fd(context->ssl, context->sockfd);

    if (SSL_connect(context->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(context->ssl);
        return -1;
    }

    return 0;
}