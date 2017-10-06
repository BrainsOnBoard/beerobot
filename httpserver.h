#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <cstddef>
#include <thread>

// for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

const int MAX_THREADS = 5;

class HttpServer;

struct connthread {
    pthread_t thread;
    HttpServer* server;
    int connfd;
    bool isrunning = false;
};

class HttpServer {
public:
    HttpServer(int port);
    void serve(bool (*handle_request)(int, char*), void (*kill_request)());
    virtual ~HttpServer();

    bool running = false;
    void (*kill_request)() = NULL;
    bool (*handle_request)(int, char*) = NULL;
private:
    int listenfd;
    connthread threads[MAX_THREADS];

    static void* run_thread(void* ptr);
};

#endif /* HTTPSERVER_H */