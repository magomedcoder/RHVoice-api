#pragma once

#include "tts_request.h"
#include <vector>
#include <cstdint>

class ITTSService {
public:
    virtual std::vector<uint8_t> Synthesize(const TTSRequest &req) = 0;

    virtual ~ITTSService() = default;
};
