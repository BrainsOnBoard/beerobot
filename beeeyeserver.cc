#include <iostream>
#include <fstream>
#include <signal.h>
#include <thread>

#include "beeeyeserver.h"

using namespace std;
using namespace cv;

#define LISTEN_PORT 1234

bool BeeEyeServer::run_request;
BeeEyeServer* BeeEyeServer::Instance;

BeeEyeServer::BeeEyeServer(Motor* mtr) : HttpServer(LISTEN_PORT), eye(get_usb())
{
    this->mtr = mtr;
}

bool getfloat(const string str, float &f)
{
    bool ret = false;
    try {
        f = stof(str, NULL);
        ret = f >= -1.0 && f <= 1.0;
    } catch (const std::invalid_argument& ia) {
    }
    if (!ret) {
        cerr << "Bad value: " << str << endl;
    }
    return ret;
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
    } else if (string(path).compare(0, 6, "/move?") == 0) {
        string spath = string(path);
        float move[2];
        int param = 3;
        int eq;
        for (int i = 6, start = 6; i < spath.length(); i++) {
            if (spath[i] == '=' && i - start > 0) {
                eq = i + 1;
                string pname = spath.substr(start, i - start);
                if (pname == "l") {
                    param = 0;
                } else if (pname == "r") {
                    param = 1;
                } else {
                    cout << "Bad URL parameter: " << pname << endl;
                    param = 2;
                }
            } else if (spath[i] == '&') {
                if (param < 2) {
                    if (!getfloat(spath.substr(eq, i - eq), move[param])) {
                        closeconn = true;
                        break;
                    }
                }
                start = i + 1;
                param = 3;
            }
        }
        if (!closeconn && param < 2) {
            closeconn = !getfloat(spath.substr(eq), move[param]);
        }
        if (!closeconn) {
            if (mtr) {
                mtr->tank(move[0], move[1]);
            } else {
                cout << "Motor not connected" << endl;
                cout << "MOVE " << move[0] << ", " << move[1] << endl;
            }
        }

        const char* msg = "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 0\r\n\r\n";
        send(connfd, msg, strlen(msg), MSG_NOSIGNAL);
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

void* BeeEyeServer::run_server(void* mtr)
{
    BeeEyeServer::run_request = true;
    BeeEyeServer::Instance = new BeeEyeServer((Motor*) mtr);
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
