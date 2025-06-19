#pragma once

#include <cpprest/http_listener.h>
#include "../usecase/tts_usecase.h"

class HttpHandler {
public:
    explicit HttpHandler(ITTSUseCase *usecase);

    void HandlePost(web::http::http_request request);

private:
    ITTSUseCase *usecase_;
};
