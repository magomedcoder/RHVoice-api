#include "rhvoice_synthesizer.h"
#include "../utils/temp_file.h"
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <sstream>

std::vector<uint8_t> RHVoiceSynthesizer::Synthesize(const TTSRequest &req) {
    std::string path = generate_temp_filename();
    int pipefd[2];
    if (pipe(pipefd) == -1) throw std::runtime_error("Ошибка создания пайпа");

    pid_t pid = fork();
    if (pid == -1) throw std::runtime_error("Ошибка fork");

    if (pid == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        execlp("RHVoice-test", "RHVoice-test", "-p", req.voice.c_str(), "-o", path.c_str(), nullptr);
        _exit(1);
    } else {
        close(pipefd[0]);
        write(pipefd[1], req.text.c_str(), req.text.size());
        close(pipefd[1]);
        waitpid(pid, nullptr, 0);
    }

    std::ifstream file(path, std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), {});
    std::remove(path.c_str());
    return data;
}
