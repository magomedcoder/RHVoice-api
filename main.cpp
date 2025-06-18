#include "src/presentation/http_handler.h"
#include "src/infrastructure/rhvoice_synthesizer.h"
#include <cpprest/http_listener.h>
#include <thread>

using namespace web;
using namespace http;

int main() {
    RHVoiceSynthesizer tts_service;
    HttpHandler handler(&tts_service);

    http::experimental::listener::http_listener listener(U("http://0.0.0.0:8000/api/v1/tts"));
    listener.support(methods::POST, std::bind(&HttpHandler::HandlePost, &handler, std::placeholders::_1));

    listener.open().then([] { std::cout << "Сервер запущен\n"; }).wait();
    while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
}
