#include "src/presentation/http_handler.h"
#include "src/usecase/tts_usecase.cpp"
#include <cpprest/http_listener.h>

int main() {
    TTSUseCase usecase(8);
    HttpHandler handler(&usecase);

    web::http::experimental::listener::http_listener listener(U("http://0.0.0.0:8000/api/v1/tts"));
    listener.support(web::http::methods::POST, std::bind(&HttpHandler::HandlePost, &handler, std::placeholders::_1));

    listener.open().then([] { std::cout << "Сервер запущен\n"; }).wait();
    while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
}
