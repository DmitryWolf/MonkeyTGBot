#ifndef CONNECTION_H
#define CONNECTION_H

#include <openssl/ssl.h>

typedef struct {
    SSL_CTX *ctx_;
    SSL *ssl_;
    int sockfd_;
} connection;

// Initialize SSL library (should be called once in a multi-threaded program)
void SSL_init();

// Destroy SSL library (should be called once in a multi-threaded program)
void SSL_destroy();

// Constructor
int connection_init(connection* context, const char *host, const char *port);

// Restart connection
int connection_restart(connection* context, const char *host, const char *port);

// Destructor
void connection_destroy(connection* context);

// Initialize SSL context
SSL_CTX *init_ssl_context();

// SSL_new + SSL_set_fd + SSL_connect
int make_SSL_connection(connection* context);

// Create socket connection
int create_socket_connection(const char *host, const char *port);

#endif // CONNECTION_H