
/*
 * File:   mainclient.h
 * Author: ad374
 *
 * Created on 13 October 2017, 17:41
 */

#pragma once

#include <string>

class MainClient {
public:
    MainClient(const std::string host, const int port);
    virtual ~MainClient();
    static void run_client(MainClient *client);
private:
    void run();
};