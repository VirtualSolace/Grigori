#pragma once

#include <string>

class Net {
    private:
	int sock;

    public:
	Net();
	virtual ~Net();

	int init(const std::string &host, int port);
	int teardown();
	bool sendData(const std::string &data);
};
