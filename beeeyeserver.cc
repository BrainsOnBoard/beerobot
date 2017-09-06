#include <iostream>
#include <fstream>
#include <signal.h>

#include "beeeyeserver.h"

using namespace std;
using namespace cv;

#define LISTEN_PORT 1234

bool BeeEyeServer::run_request;
BeeEyeServer* BeeEyeServer::Instance;

BeeEyeServer::BeeEyeServer() : HttpServer(LISTEN_PORT), eye(VIDEO_DEV)
{
}

bool BeeEyeServer::handle_request(int connfd, char* path)
{
    bool closeconn = false;

    cout << "Requested: " << path << endl;

    if (strcmp(path, "/") == 0) {
        ifstream fs("index.html");
        if (fs.is_open()) {
            // get file length
            fs.seekg(0, ios::end);
            int length = fs.tellg();
            fs.seekg(0, ios::beg);

            // read file into buffer
            char buff[length];
            fs.read(buff, length);

            // send along to client along with HTTP header
            const string header = "HTTP/1.1 200 OK\r\n"
                    "Content-type: text/html\r\n"
                    "Content-length: " + to_string(length) + "\r\n\r\n";
            if (send(connfd, header.c_str(), header.length(), MSG_NOSIGNAL) == -1 ||
                    send(connfd, buff, length, MSG_NOSIGNAL) == -1) {
                cerr << "Error writing" << endl;
                closeconn = true;
            }
        }
    } else if (strcmp(path, "/stream.mjpg") == 0) {
        const char* msg = "HTTP/1.1 200 OK\r\n"
                "Content-Type: multipart/x-mixed-replace;boundary=--jpegboundary\r\n"
                "Content-Encoding: identity\r\n"
                "Cache-Control: no-cache\r\n"
                "Max-Age: 0\r\n"
                "Expires: 0\r\n"
                "Pragma: no-cache\r\n"
                "Connection: close\r\n\r\n";
        if (send(connfd, msg, strlen(msg), MSG_NOSIGNAL) == -1) {
            cerr << "Error writing" << endl;
            closeconn = true;
            goto close;
        }

        // input and final output image matrices
        Mat view;
        vector<uchar> buff;
        while (run_request) {
            if (!eye.get_eye_view(view)) {
                cerr << "Error: Could not read from webcam" << endl;
                closeconn = true;
                goto close;
            }

            // convert image to JPEG; store in buff
            imencode(".jpg", view, buff);

            // send a header + JPEG data
            const string header = "--jpegboundary\r\n"
                    "Content-Type: image/jpeg\r\n"
                    "Content-Length: " + to_string(buff.size()) + "\r\n\r\n";
            int val = send(connfd, header.c_str(), header.length(), MSG_NOSIGNAL);
            if (val == -1) {
                cerr << "Error writing JPEG header" << endl;
                closeconn = true;
                break;
            }
            if (send(connfd, buff.data(), buff.size(), MSG_NOSIGNAL) == -1) {
                cerr << "Error writing JPEG data" << endl;
                closeconn = true;
                break;
            }
        }
    } else {
        const char* msg = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(connfd, msg, strlen(msg), MSG_NOSIGNAL);
        cout << "404 page not found" << endl;
        closeconn = true;
    }

close:
    if (!run_request || closeconn) {
        cout << "Closing connection " << connfd << endl;
        close(connfd);

        return true;
    }
    return false;
}

void BeeEyeServer::run_server()
{
    BeeEyeServer::run_request = true;
    BeeEyeServer::Instance = new BeeEyeServer();
    BeeEyeServer::Instance->run();
}

void BeeEyeServer::run()
{
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = [](int)
    {
        //BeeEyeServer::stop_server();
        exit(0);
    };
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    serve(&handle_request_server, NULL);
}

bool BeeEyeServer::handle_request_server(int connfd, char* path)
{
    return BeeEyeServer::Instance->handle_request(connfd, path);
}

void BeeEyeServer::kill_request_server()
{
    BeeEyeServer::run_request = false;
}

void BeeEyeServer::stop_server()
{
    BeeEyeServer::Instance->~HttpServer();
}