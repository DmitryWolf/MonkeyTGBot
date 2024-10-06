#ifndef CONNECTION_H
#define CONNECTION_H

#include <openssl/ssl.h>

typedef struct {
    SSL_CTX *ctx;
    SSL *ssl;
    int sockfd;
} connection;

// Constructor
int connection_init(connection* context, const char *host, const char *port, int type);

// Destructor
void connection_destroy(connection* context);

// Initialize SSL context
SSL_CTX *init_ssl_context();

// SSL_new + SSL_set_fd + SSL_connect
int make_SSL_connection(connection* context);

// Create socket connection
int create_socket_connection(const char *host, const char *port);

#endif // CONNECTION_H