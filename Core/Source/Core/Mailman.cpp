#include "Mailman.h"
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace Instrument {
    Mailman::Mailman() {
        fd = open(device.c_str(), O_RDWR);
        if (fd < 0) {
            throw std::runtime_error("Erro ao abrir dispositivo " + device + ": " + strerror(errno));
        }
    }

    Mailman::~Mailman() {
        if (fd >= 0) {
            close(fd);
        }
    }

    void Mailman::Send(const std::string& cmd) {
        if (fd < 0) {
            throw std::runtime_error("Dispositivo não está aberto para envio.");
        }
        ssize_t written = write(fd, cmd.c_str(), cmd.size());
        if (written < 0) {
            throw std::runtime_error("Erro ao enviar comando: " + std::string(strerror(errno)));
        }
    }

    std::string Mailman::Receive() {
        if (fd < 0) {
            throw std::runtime_error("Dispositivo não está aberto para recebimento.");
        }

        char buffer[4096] = {0};
        ssize_t read_bytes = read(fd, buffer, sizeof(buffer) - 1);
        buffer[read_bytes] = '\0';

        std::string result(buffer);
        size_t endpos = result.find_last_not_of("\r\n \t");
        if (std::string::npos != endpos) {
            result = result.substr(0, endpos + 1);
        }

        return result;
    }

    std::string Mailman::Query(const std::string& cmd) {
        Send(cmd);
        usleep(100000);
        return Receive();
    }
}
