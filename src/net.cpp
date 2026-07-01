#include "../headers/net.h"
#include "../headers/debug.h"
#include "sys/socket.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

Net::Net()
	: sock(-1)
{
}
Net::~Net()
{
	teardown();
}

int Net::init(const std::string &host, int port)
{
	struct addrinfo hints{};
	struct addrinfo *result = nullptr;

	hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;

	std::string portStr = std::to_string(port);

	int ret = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);

	if (ret != 0)
		return -1;

	for (auto *rp = result; rp != nullptr; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

		if (sock < 0)
			continue;

		if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) {
			freeaddrinfo(result);
			return sock;
		}

		close(sock);
		sock = -1;
	}

	freeaddrinfo(result);
	return -1;
}

int Net::teardown()
{
	if (sock >= 0) {
		if (close(sock) < 0)
			return -1;
		sock = -1;
	}
	return 0;
}

bool Net::sendData(const std::string &data)
{
	ssize_t bytes = send(sock, data.data(), data.size(), MSG_NOSIGNAL);
	return bytes > 0;
}

/*
int Net::recvData(std::string& data){
    ssize_t bytes = read(sock, data.data(), data.size());
    return bytes > 0;
}
*/
