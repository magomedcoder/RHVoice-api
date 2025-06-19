#include "http_handler.h"
#include <cpprest/json.h>

HttpHandler::HttpHandler(ITTSUseCase *usecase) : usecase_(usecase) {}

void HttpHandler::HandlePost(web::http::http_request request) {
    request.extract_json().then([this, request](web::json::value val) mutable {
        try {
            if (!val.has_field("text") || !val.at("text").is_string()) {
                web::json::value err;
                err["error"] = web::json::value::string("Параметр 'text' обязателен");
                request.reply(web::http::status_codes::BadRequest, err);
                return;
            }

            TTSRequest req;
            req.text = val.at("text").as_string();
            if (val.has_field("voice") && val.at("voice").is_string())
                req.voice = val.at("voice").as_string();

            auto future = usecase_->ProcessRequest(req);

            std::thread([future = std::move(future), request]() mutable {
                try {
                    auto data = future.get();
                    web::http::http_response response(web::http::status_codes::OK);
                    response.headers().add("Content-Type", "audio/wav");
                    response.set_body(concurrency::streams::bytestream::open_istream(std::move(data)));
                    request.reply(response);
                } catch (const std::exception &e) {
                    web::json::value err;
                    err["error"] = web::json::value::string(e.what());
                    request.reply(web::http::status_codes::InternalError, err);
                }
            }).detach();

        } catch (...) {
            web::json::value err;
            err["error"] = web::json::value::string("Неизвестная ошибка при обработке запроса");
            request.reply(web::http::status_codes::InternalError, err);
        }
    });
}
