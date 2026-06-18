#include "../headers/utils.h"
#include "../headers/debug.h"
#include <csignal>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    volatile sig_atomic_t g_sigterm_received = 0;

    void handle_sigterm(int){
        g_sigterm_received = 1;
    }
}

namespace utils
{
    std::string decryptor(const uint8_t* data, const uint8_t* k, size_t length){
        std::string output;
        output.reserve(length);

        for (size_t i = 0; i < length; i++)
        {
            output.push_back(static_cast<char>(data[i] ^ k[i]));
        }

        return output;
    }

    int createMFile(const std::string& filename,
                    const std::vector<std::string>& results)
{
        int fd = memfd_create(filename.c_str(), MFD_CLOEXEC);
        if (fd == -1)
            return -1;

        for (const auto& line : results){
            write(fd, line.c_str(), line.size());
            write(fd, "\n", 1);
        }

        lseek(fd, 0, SEEK_SET);
        return fd;
    }

    int readFile(const std::string& filename)
    {
        int fd = open(filename.c_str(), O_RDONLY);
        if (fd < 0) return -1;

        std::string buffer;
        char tmp[4096];
        ssize_t bytes;

        while ((bytes = read(fd, tmp, sizeof(tmp))) > 0)
        {
            buffer.append(tmp, bytes);
        }

        close(fd);

        if (bytes < 0)
            return -1;

        std::vector<std::string> data;
        std::string current;

        for (char c : buffer)
        {
            if (c == '\n')
            {
                data.push_back(current);
                current.clear();
            }
            else
            {
                current.push_back(c);
            }
        }

        if (!current.empty())
            data.push_back(current);

        return createMFile(filename, data);
    }

    int writeFile(const std::string& filename, std::string& jsonData)
    {
        int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
            return -1;

        ssize_t written = 0;
        size_t total = 0;

        while (total < jsonData.size())
        {
            written = write(fd, jsonData.data() + total, jsonData.size() - total);

            if (written <= 0)
            {
                close(fd);
                return -1;
            }

            total += written;
        }
        write(fd, "\n", 1);

        close(fd);
        return 0;
    }

    std::string serializeVectorToJson(const std::vector<std::string>& vec) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << "\"" << vec[i] << "\"";
            if (i + 1 != vec.size()) oss << ",";
        }
        oss << "]";
        return oss.str();
    }

    void sigterm_handler()
    {
        signal(SIGTERM, handle_sigterm);
    }

    bool received_sigterm()
    {
        return g_sigterm_received != 0;
    }
}
