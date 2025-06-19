#include "http_handler.h"
#include "../domain/tts_request.h"
#include <cpprest/json.h>
#include <cpprest/http_listener.h>

using namespace web;
using namespace http;

HttpHandler::HttpHandler(ITTSService *tts_service) : tts_service_(tts_service) {}

void HttpHandler::HandlePost(http_request request) {
    request.extract_json().then([this, request](json::value val) mutable {
        try {
            if (!val.has_field("text") || !val.at("text").is_string()) {
                json::value error;
                error["error"] = json::value::string("Параметр 'text' обязателен");
                request.reply(status_codes::BadRequest, error);
                return;
            }

            TTSRequest req;
            req.text = val.at("text").as_string();
            if (val.has_field("voice") && val.at("voice").is_string())
                req.voice = val.at("voice").as_string();

            auto data = tts_service_->Synthesize(req);

            http_response response(status_codes::OK);
            response.headers().add("Content-Type", "audio/wav");
            response.headers().add("Content-Disposition", "attachment; filename=output.wav");
            response.set_body(concurrency::streams::bytestream::open_istream(std::move(data)));
            request.reply(response);

        } catch (const std::exception &e) {
            json::value error;
            error["error"] = json::value::string(std::string("Ошибка: ") + e.what());
            request.reply(status_codes::InternalError, error);
        } catch (...) {
            json::value error;
            error["error"] = json::value::string("Неизвестная ошибка при обработке запроса");
            request.reply(status_codes::InternalError, error);
        }
    });
}
