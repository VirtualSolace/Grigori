#include "../headers/https.h"
#include "../headers/info.h"
#include "../headers/debug.h"

#include <curl/curl.h>
#include <sstream>
#include <vector>
#include <format>

Https::Https() : curl(nullptr) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

Https::~Https() {
  teardown();
  curl_global_cleanup();
}

int Https::httpsInit(const std::string& ip) {
  path = ePath;
  host = eHost;
  AT   = eAT;

  // Override DNS resolution with an IP:
  serverIP = ip;

  curl = curl_easy_init();
  if (!curl)
    return -1;

  return 0;
}

int Https::teardown() {
  if (curl) {
    curl_easy_cleanup(curl);
    curl = nullptr;
  }

  responseBuffer.clear();
  return 0;
}

size_t Https::writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
  std::string* buffer = static_cast<std::string*>(userdata);
  buffer->append(ptr, size * nmemb);

  return size * nmemb;
}

int Https::sendRequest(const std::string& url, const std::string& method, const std::string& body, bool keepAlive) {
  if (!curl) return -1;

  responseBuffer.clear();

  curl_easy_reset(curl);

  struct curl_slist* headers = nullptr;

  headers = curl_slist_append(
    headers,
    std::format("X-Access-Token: {}", AT).c_str());

  headers = curl_slist_append(
    headers,
    "Content-Type: application/json");

  headers = curl_slist_append(
    headers,
    keepAlive ? "Connection: keep-alive"
    : "Connection: close");

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

  // force DNS resolution
  struct curl_slist* resolve = nullptr;
  if (!serverIP.empty()) {
    std::string mapping =
    std::format("{}:{}:{}", host, PORT, serverIP);

    resolve = curl_slist_append(resolve, mapping.c_str());
    curl_easy_setopt(curl, CURLOPT_RESOLVE, resolve);
  }

  if (method == "POST") {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl,
                     CURLOPT_POSTFIELDS,
                     body.c_str());

    curl_easy_setopt(curl,
                     CURLOPT_POSTFIELDSIZE,
                     body.size());
  }
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
  CURLcode result = curl_easy_perform(curl);

  curl_slist_free_all(headers);

  if (resolve) curl_slist_free_all(resolve);

  if (result != CURLE_OK) return -1;

  long responseCode = 0;
  curl_easy_getinfo(curl,
                    CURLINFO_RESPONSE_CODE,
                    &responseCode);

  return (responseCode >= 200 && responseCode < 300)
  ? 0
  : -1;
}

int Https::readResponse(std::string& response) {
  response = responseBuffer;
  return 0;
}

std::vector<std::string> Https::recvData() {
  std::vector<std::string> messages;

  std::stringstream ss(responseBuffer);
  std::string line;

  while (std::getline(ss, line)) {
    if (!line.empty()) messages.push_back(line);

  return messages;
}

int Https::craftPost(int i, int j, const std::string& postData){
  if (!curl) return -1;

  if (i < 0 || i >= (int)query.size()) return -1;

  if (j < CLOSE || j > KEEP_ALIVE) return -1;

  bool keepAlive = (j == KEEP_ALIVE);

  std::string url = std::format(
    "https://{}:{}{}?type={}",
    host,
    PORT,
    path,
    query.at(i));

  return sendRequest(url, "POST", postData, keepAlive);
}

int Https::craftGet(int i, int j) {
  if (!curl) return -1;

  if (i < 0 || i >= (int)query.size())return -1;

  if (j < CLOSE || j > KEEP_ALIVE) return -1;

  bool keepAlive = (j == KEEP_ALIVE);

  std::string url = std::format(
    "https://{}:{}{}?type={}",
    host,
    PORT,
    path,
    query.at(i));

  return sendRequest(url, "GET", "", keepAlive);
}
