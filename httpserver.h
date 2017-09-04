#ifndef HTTPSERVER_H
#define HTTPSERVER_H

// for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

class HttpServer {
public:
    HttpServer(int port);
    void serve(bool (*)(int, char*));
    virtual ~HttpServer();
private:
    int listenfd;
};

#endif /* HTTPSERVER_H */