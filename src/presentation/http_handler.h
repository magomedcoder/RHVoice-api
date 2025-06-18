#pragma once

#include <cpprest/http_listener.h>
#include "../domain/tts_service_interface.h"

class HttpHandler {
public:
    explicit HttpHandler(ITTSService *tts_service);

    void HandlePost(web::http::http_request request);

private:
    ITTSService *tts_service_;
};
