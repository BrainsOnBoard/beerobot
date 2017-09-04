#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <cstddef>

// for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

class HttpServer {
public:
    HttpServer(int port);
    void serve(bool (*handle_request)(int, char*), void (*kill_request)());
    virtual ~HttpServer();
private:
    int listenfd;
    bool running = false;
    void (*kill_request)() = NULL;
};

#endif /* HTTPSERVER_H */