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
    context->ctx_ = init_ssl_context();
    if (!context->ctx_) {
        return -1;
    }

    context->sockfd_ = create_socket_connection(host, port);
    if (context->sockfd_ == -1) {
        SSL_CTX_free(context->ctx_);
        return -1;
    }

    if (make_SSL_connection(context) == -1) {
        close(context->sockfd_);
        SSL_CTX_free(context->ctx_);
        return -1;
    }
    return 0;
}

void connection_destroy(connection* context) {
    if (context->ssl_) {
        int shutdown_state = SSL_get_shutdown(context->ssl_);
        if (shutdown_state == 0) {
            int ret = SSL_shutdown(context->ssl_);
            if (ret == 0) {
                ret = SSL_shutdown(context->ssl_);
            }
            if (ret < 0) {
                int ssl_err = SSL_get_error(context->ssl_, ret);
                if (ssl_err == SSL_ERROR_SYSCALL || ssl_err == SSL_ERROR_SSL) {
                    ERR_print_errors_fp(stderr);
                }
            }
        }
        SSL_free(context->ssl_);
        context->ssl_ = NULL;
    }

    if (context->sockfd_ != -1) {
        close(context->sockfd_);
        context->sockfd_ = -1;
    }

    if (context->ctx_) {
        SSL_CTX_free(context->ctx_);
        context->ctx_ = NULL;
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

    // Set up options SSL_OP_IGNORE_UNEXPECTED_EOF
    long options = SSL_CTX_get_options(ctx);
    options |= SSL_OP_IGNORE_UNEXPECTED_EOF;
    SSL_CTX_set_options(ctx, options);

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
    context->ssl_ = SSL_new(context->ctx_);
    if (!context->ssl_) {
        perror("Unable to create SSL structure");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    SSL_set_fd(context->ssl_, context->sockfd_);

    if (SSL_connect(context->ssl_) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(context->ssl_);
        return -1;
    }

    return 0;
}