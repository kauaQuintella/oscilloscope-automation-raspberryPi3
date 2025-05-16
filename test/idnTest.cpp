/*
CÓDIGO HELLO WORLD FEITO PELO GPT

g++ -o exe/idnCpp test/idnTest.cpp
./idnCpp

expected: TEKTRONIX,TDS 2024C,C042765,CF:91.1CT FV:v24.26

*/

#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

int main() {
    const std::string device = "/dev/usbtmc0";
    const std::string cmd = "*IDN?\n";
    char buffer[256] = {0};

    int fd = open(device.c_str(), O_RDWR);
    if (fd < 0) {
        std::cerr << "Erro ao abrir dispositivo: " << strerror(errno) << std::endl;
        return 1;
    }

    ssize_t written = write(fd, cmd.c_str(), cmd.size());
    if (written < 0) {
        std::cerr << "Erro ao enviar comando: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    usleep(100000);  // 100ms de espera

    ssize_t read_bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (read_bytes < 0) {
        std::cerr << "Erro ao ler resposta: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    buffer[read_bytes] = '\0';
    std::cout << "Resposta: " << buffer << std::endl;

    close(fd);
    return 0;
}
