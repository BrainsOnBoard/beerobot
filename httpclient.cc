#include "httpclient.h"

#include <iostream>

#include <cassert>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <opencv2/opencv.hpp>

using namespace std;

HttpClient::HttpClient(const std::string &address, int port) : m_Socket(-1), m_HttpHeaderRead(false)
{
    // Create socket
    m_Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_Socket < 0) {
        throw std::runtime_error("Cannot open socket");
    }

    // Create socket address structure
    sockaddr_in destAddress = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr =
        { .s_addr = inet_addr(address.c_str())}
    };

    // Connect socket
    if (connect(m_Socket, reinterpret_cast<sockaddr*> (&destAddress), sizeof (destAddress)) < 0) {
        throw std::runtime_error("Cannot connect socket to " + address + ":" + std::to_string(port));
    }

    printf("Connected\n");

    // Make get request
    const char *getRequest = "GET /stream.mjpg HTTP /1.1\r\n";
    if (write(m_Socket, getRequest, strlen(getRequest)) < 0) {
        throw std::runtime_error("Cannot make GET request");
    }

    printf("Opened socket\n");
}

HttpClient::~HttpClient()
{
    if (m_Socket > 0) {
        close(m_Socket);
    }
}

bool HttpClient::get_image(cv::Mat &imorig)
{
    char buffer[1024];
    bool jpegHeadersRead = false;

    size_t dataOffset = 0;
    size_t contentToRead = 0;
    while (true) {
        // Loop through buffer characters
        ssize_t bufferPos = 0;
        size_t bytesToRead = (contentToRead == 0) ? 1024 : std::min<size_t>(contentToRead, 1024);
        // Read buffer full of socket data
        const ssize_t len = read(m_Socket, buffer, bytesToRead);
        if (len == 0) {
            continue;
        } else if (len < 0) {
            std::cerr << "Cannot read from socket err:" << strerror(errno) << std::endl;
            return false;
        }

        // If we haven't yet read HTTP headers
        if (!m_HttpHeaderRead) {
            size_t currentHeaderPosition = 0;
            char currentHeader[256];

            while (bufferPos < len) {
                // If this is the start of a header termination character
                if (buffer[bufferPos] == '\r' && buffer[bufferPos + 1] == '\n') {
                    // If this was a double terminator
                    if (currentHeaderPosition == 0) {
                        m_HttpHeaderRead = true;

                        // Skip over terminators
                        bufferPos += 2;

                        break;
                    }

                    // Add newline and print header
                    currentHeader[currentHeaderPosition + 1] = 0;

                    // Reset header position and advance over
                    currentHeaderPosition = 0;

                    // Skip over terminators
                    bufferPos += 2;
                }// Otherwise copy character to current header buffer
                else {
                    assert(currentHeaderPosition < 256);
                    currentHeader[currentHeaderPosition++] = buffer[bufferPos++];
                }
            }

            assert(bufferPos == len);
            assert(m_HttpHeaderRead);
        }// If we haven't yet read JPEG headers
        else if (!jpegHeadersRead) {
            size_t currentHeaderPosition = 0;
            char currentHeader[256];

            while (bufferPos < len) {
                // If this is the start of a header termination character
                if (buffer[bufferPos] == '\r' && buffer[bufferPos + 1] == '\n') {
                    // If this was a double terminator
                    if (currentHeaderPosition == 0) {
                        jpegHeadersRead = true;

                        // Skip over terminators
                        bufferPos += 2;

                        break;
                    }

                    // Add newline and print header
                    currentHeader[currentHeaderPosition] = 0;
                    if (strstr(currentHeader, "Content-Length") == currentHeader) {
                        contentToRead = (size_t) atoi(currentHeader + 16);
                        m_ImageData.resize(contentToRead);
                    }

                    // Reset header position and advance over
                    currentHeaderPosition = 0;

                    // Skip over terminators
                    bufferPos += 2;
                }// Otherwise copy character to current header buffer
                else {
                    assert(currentHeaderPosition < 256);
                    currentHeader[currentHeaderPosition++] = buffer[bufferPos++];
                }
            }
            assert(jpegHeadersRead);
        }

        if (jpegHeadersRead && bufferPos < len) {
            // Image data
            const size_t bytesToCopy = len - bufferPos;

            memcpy(&m_ImageData[dataOffset], &buffer[bufferPos], bytesToCopy);

            contentToRead -= bytesToCopy;
            dataOffset += bytesToCopy;

            if (dataOffset >= m_ImageData.size()) {
                cv::imdecode(m_ImageData, cv::IMREAD_COLOR, &imorig);
                return true;
            }
        }
    }
}

void HttpClient::tank(float left, float right)
{
    cout << "HTTP: left: " << left << ", right: " << right << endl;

    // Make get request
    const string getRequest = "GET /move?l=" + to_string(left) + "&r=" + to_string(right) + " HTTP/1.1\r\n";
    if (write(m_Socket, getRequest.c_str(), getRequest.length()) < 0) {
        throw std::runtime_error("Cannot make GET request");
    }
}