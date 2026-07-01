#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

static constexpr int PORT = 2213;

class Https {
    private:
	CURL *curl;

	std::string path;
	std::string host;
	std::string AT;

	std::string response;

	static size_t writeCallback(void *contents, size_t size, size_t nmemb,
				    void *userp);

    public:
	Https();
	~Https();

	int httpsInit(const std::string &ip);
	int teardown();

	int craftGet(int i, int j);
	int craftPost(int i, int j, const std::string &postData = "");

	std::vector<std::string> recvData();
};
