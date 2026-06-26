#pragma once

#include "net.h"

#include <curl/curl.h>
#include <string>
#include <vector>

static constexpr int PORT = 2213;

class Https : public Net {
private:
    CURL* curl;

    std::string path;
    std::string host;
    std::string AT;

    // Stores the full HTTP response body.
    std::string responseBuffer;

    std::string serverIP;

    static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);

    int sendRequest(const std::string& url, const std::string& method, const std::string& body, bool keepAlive);

public:
    Https();
    ~Https() override;

    int httpsInit(const std::string& ip);

    int teardown() override;

    int craftGet(int i, int j);

    int craftPost(int i, int j, const std::string& postData = "");

    int readResponse(std::string& response);

    std::vector<std::string> recvData();
};
