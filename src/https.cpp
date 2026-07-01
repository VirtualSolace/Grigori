#include "../headers/https.h"
#include "../headers/info.h"

#include <curl/curl.h>
#include <sstream>
#include <format>

Https::Https()
	: curl(nullptr)
{
}

Https::~Https()
{
	teardown();
}

size_t Https::writeCallback(void *contents, size_t size, size_t nmemb,
			    void *userp)
{
	size_t total = size * nmemb;
	static_cast<std::string *>(userp)->append(static_cast<char *>(contents),
						  total);
	return total;
}

int Https::httpsInit(const std::string &ip)
{
	path = ePath;
	host = eHost;
	AT = eAT;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (!curl)
		return -1;

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	std::string url = std::format("https://{}:{}{}", ip, PORT, path);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	return 0;
}

int Https::teardown()
{
	if (curl) {
		curl_easy_cleanup(curl);
		curl = nullptr;
	}

	curl_global_cleanup();

	return 0;
}

int Https::craftGet(int i, int j)
{
	if (!curl)
		return -1;

	response.clear();

	std::string url = std::format("https://{}:{}{}?type={}", host, PORT,
				      path, query[i]);

	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	struct curl_slist *headers = nullptr;

	headers = curl_slist_append(
		headers, std::format("Connection: {}", conn_status[j]).c_str());
	headers = curl_slist_append(
		headers, std::format("X-Access-Token: {}", AT).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CURLcode res = curl_easy_perform(curl);

	curl_slist_free_all(headers);

	return (res == CURLE_OK) ? 0 : -1;
}

std::vector<std::string> Https::recvData()
{
	std::vector<std::string> messages;

	std::stringstream ss(response);
	std::string line;

	while (std::getline(ss, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (!line.empty())
			messages.push_back(line);
	}

	return messages;
}

int Https::craftPost(int i, int j, const std::string &postData)
{
	if (!curl)
		return -1;

	response.clear();

	std::string url = std::format("https://{}:{}{}?type={}", host, PORT,
				      path, query[i]);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());

	struct curl_slist *headers = nullptr;

	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(
		headers, std::format("Connection: {}", conn_status[j]).c_str());

	headers = curl_slist_append(
		headers, std::format("X-Access-Token: {}", AT).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	CURLcode res = curl_easy_perform(curl);

	curl_slist_free_all(headers);

	return (res == CURLE_OK) ? 0 : -1;
}
