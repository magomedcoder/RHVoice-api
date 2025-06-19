#pragma once

#include "../domain/tts_service_interface.h"
#include <future>

class ITTSUseCase {
public:
    virtual std::future <std::vector<uint8_t>> ProcessRequest(const TTSRequest &request) = 0;

    virtual ~ITTSUseCase() = default;
};
