#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace utils
{
  std::string decryptor(const uint8_t* data, const uint8_t* k, size_t length);
  std::string serializeVectorToJson(const std::vector<std::string>& vec);

  int createMFile(const std::string& filename,
                  const std::vector<std::string>& results);
  int readFile(const std::string& filename);
  int writeFile(const std::string& filename, std::string& jsonData);

  void sigterm_handler();
  bool received_sigterm();

}
