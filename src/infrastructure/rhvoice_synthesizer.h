#pragma once

#include "../domain/tts_service_interface.h"

class RHVoiceSynthesizer : public ITTSService {
public:
    std::vector<uint8_t> Synthesize(const TTSRequest &req) override;
};
