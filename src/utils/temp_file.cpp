#include "temp_file.h"
#include <stdexcept>
#include <unistd.h>

std::string generate_temp_filename() {
    char filename[] = "/tmp/outputXXXXXX.wav";
    int fd = mkstemps(filename, 4);
    if (fd == -1) {
        throw std::runtime_error("Не удалось создать временный файл");
    }
    close(fd);
    return std::string(filename);
}
