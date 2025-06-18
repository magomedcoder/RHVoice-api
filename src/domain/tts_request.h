#pragma once

#include <string>

struct TTSRequest {
    std::string text;
    std::string voice = "Anna";
};
