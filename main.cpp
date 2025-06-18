#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <thread>

using namespace web;
using namespace http;
using namespace http::experimental::listener;

std::string generate_temp_filename() {
    char filename[] = "/tmp/outputXXXXXX.wav";
    int fd = mkstemps(filename, 4);
    if (fd == -1) {
        throw std::runtime_error("Не удалось создать временный файл");
    }
    close(fd);
    return std::string(filename);
}

void handle_post(http_request request) {
    request.extract_json().then([request](pplx::task <json::value> task) {
        try {
            json::value req_json = task.get();
            if (!req_json.has_field("text") || !req_json.at("text").is_string()) {
                request.reply(status_codes::BadRequest, U("Параметр 'text' обязателен"));
                return;
            }

            std::string text = req_json.at("text").as_string();
            std::string voice = "Anna";
            std::string outPath = generate_temp_filename();

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                request.reply(status_codes::InternalError, U("Ошибка создания пайпа"));
                return;
            }

            pid_t pid = fork();
            if (pid == -1) {
                request.reply(status_codes::InternalError, U("Ошибка fork"));
                return;
            }

            if (pid == 0) {
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[1]);

                execlp("RHVoice-test", "RHVoice-test", "-p", voice.c_str(), "-o", outPath.c_str(), nullptr);
                perror("exec failed");
                _exit(1);
            } else {
                close(pipefd[0]);
                write(pipefd[1], text.c_str(), text.length());
                close(pipefd[1]);
                waitpid(pid, nullptr, 0);

                std::ifstream audioFile(outPath, std::ios::binary);
                if (!audioFile.is_open()) {
                    request.reply(status_codes::InternalError, U("Ошибка чтения аудиофайла"));
                    return;
                }

                std::ostringstream ss;
                ss << audioFile.rdbuf();
                audioFile.close();
                std::remove(outPath.c_str());

                http_response response(status_codes::OK);
                response.headers().add(U("Content-Type"), U("audio/wav"));
                response.headers().add(U("Content-Disposition"), U("attachment; filename=output.wav"));
                response.set_body(ss.str(), "audio/wav");
                request.reply(response);
            }

        } catch (const std::exception &e) {
            request.reply(status_codes::BadRequest, U("Ошибка JSON или синтеза речи"));
        }
    });
}

int main() {
    http_listener listener(U("http://0.0.0.0:8080/tts"));
    listener.support(methods::POST, handle_post);

    try {
        listener
                .open()
                .then([]() { std::cout << "Сервер запущен\n"; })
                .wait();

        while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
    } catch (std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
