#pragma once

#include <string>
#include <array>

#define TEMP_FILENAME ".temp.grigori"
#define IP "127.0.0.1"

const std::string ePath = "/godish";
const std::string eHost = IP;
const std::string eAT =
	"i_am_the_alpha_and_the_omega_the_first_and_the_last_the_beginning_and_the_end";
const std::array<std::string_view, 3> query = { "beat", "orders", "something" };
const std::array<std::string_view, 2> conn_status = { "close", "keep-alive" };

//constexpr uint8_t ans[] = {0x43,0x68,0x61,0x69,0x6e,0x73,0x61,0x77};
