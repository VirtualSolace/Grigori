#pragma once

#include "net.h"
#include <openssl/ssl.h>
#include <string>
#include <vector>

static constexpr int PORT = 2213;

class Https : public Net {
private:
    SSL* ssl;
    SSL_CTX* ctx;

    std::string path;
    std::string host;
    std::string AT;
    std::string buffer;

public:
    Https();
    ~Https() override;

    int httpsInit(const std::string& ip);   // int port
    int teardown();

    int sendData(const std::string& data);

    int craftGet(int i, int j);

    int craftPost(int j, int i, const std::string& postData = "");

    int readResponse(std::string& response);
    std::vector<std::string> recvData();
};
