#include "mainclient.h"

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

MainClient::MainClient(const string host, const int port)
{

}

MainClient::~MainClient()
{
}

void MainClient::run()
{

}

void MainClient::run_client(MainClient* client)
{
    client->run();
}