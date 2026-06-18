#include "../headers/https.h"
#include "../headers/info.h"
#include "../headers/net.h"
#include "../headers/debug.h"
#include <openssl/ssl.h>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <format>

Https::Https() : ssl(nullptr), ctx(nullptr) {}			// path(), host(), AT(), net {} {}

Https::~Https() = default;

int Https::httpsInit(const std::string& ip)     // int port
{
  path = ePath;   //utils::decryptor(ePath, ans, sizeof(ePath));
  host = eHost;  //utils::decryptor(eHost, ans, sizeof(eHost));
  AT = eAT;     //utils::decryptor(eAT, ans, sizeof(eAT));

  int sock = init(ip, PORT);
  if (sock < 0) return -1;

  ctx = SSL_CTX_new(TLS_client_method());
  if (!ctx)
    return -1;

  ssl = SSL_new(ctx);
  if (!ssl) return -1;

  SSL_set_fd(ssl, sock);

  if (SSL_connect(ssl) <= 0)
  {
    teardown();
    return -1;
  }

  return 0;
}

int Https::teardown(){
  if(ssl){
    int result = SSL_shutdown(ssl);
    if(result == 0) SSL_shutdown(ssl);

    SSL_free(ssl);
    ssl = nullptr;
  }

  if(ctx){
    SSL_CTX_free(ctx);
    ctx = nullptr;
  }

  if(Net::teardown() < 0) return -1;
  return 0;
}

int Https::sendData(const std::string& data){
  if(ssl == nullptr) return -1;

  size_t sent = 0;

  while (sent < data.size()){
    int bytes = SSL_write(ssl, data.data() + sent, data.size() - sent);

    if (bytes <= 0) return -1;

    sent += bytes;
  }

  return 0;
}

int Https::readResponse(std::string& response) {
    if (!ssl) return -1;

    char buffer[2048];
    int bytes;

    response.clear();

    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        response += buffer;
    }

    return 0;
}

// =======================
// std::vector<std::string> Https::recvData(){}
std::vector<std::string> Https::recvData() {
    std::vector<std::string> messages;

    std::string response;
    if (readResponse(response) < 0)
        return messages;

    size_t header_end = response.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return messages;

    std::string body = response.substr(header_end + 4);

    std::stringstream ss(body);
    std::string line;

    while (std::getline(ss, line)) {
        if (!line.empty())
            messages.push_back(line);
    }

    return messages;
}
// ===========================


int Https::craftPost(int i, int j, const std::string& postData) {
  if (ssl == nullptr) return -1;

  std::string request = std::format(
    "POST {}?type={} HTTP/1.1\r\n"
    "Host: {}\r\n"
    "X-Access-Token: {}\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: {}\r\n"
    "Connection: {}\r\n\r\n"
    "{}",
    path,
    query[i],
    host,
    AT,
    static_cast<unsigned long>(postData.size()),
    conn_status[j],
    postData
  );

  return sendData(request);
}

int Https::craftGet(int i, int j){

  std::size_t size = sizeof(query) / sizeof(query[0]);
  if(ssl == nullptr) return -1;
  if(i < 0 || static_cast<std::size_t>(i) >= size) return -1;

  std::string request = std::format(
    "GET {}?type={} HTTP/1.1\r\n"
    "Host: {}\r\n"
    "X-Access-Token: {}\r\n"
    "Connection: {}\r\n\r\n",
    path,
    query[i],
    host,
    AT,
    conn_status[j]
  );

  return sendData(request);
}
