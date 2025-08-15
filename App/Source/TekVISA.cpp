using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

class Mailman {
public:
    const std::string device = "/dev/usbtmc0";
    char buffer[256] = {0};
    int fd = -1; // Inicializa com -1 para indicar que não está aberto

    // Construtor: Abre o dispositivo uma vez
    Mailman() {
        fd = open(device.c_str(), O_RDWR);
        if (fd < 0) {
            std::cerr << "Erro ao abrir dispositivo: " << strerror(errno) << std::endl;
        }
    }

    // Destrutor: Fecha o dispositivo quando o objeto é destruído
    ~Mailman() {
        if (fd >= 0) {
            close(fd);
        }
    }

    // Método para enviar um comando
    int Send(const std::string& cmd) {
        if (fd < 0) {
            std::cerr << "Dispositivo não está aberto." << std::endl;
            return 1;
        }

        // Adiciona um terminador de linha, se necessário
        std::string full_cmd = cmd + "\n";
        ssize_t written = write(fd, full_cmd.c_str(), full_cmd.size());
        if (written < 0) {
            std::cerr << "Erro ao enviar comando: " << strerror(errno) << std::endl;
            return 1;
        }

        // usleep(100000); // 100ms de espera

        return 0;
    }

    // Método para receber uma resposta
    std::string Receive() {
        if (fd < 0) {
            std::cerr << "Dispositivo não está aberto." << std::endl;
            return "";
        }
        
        ssize_t read_bytes = read(fd, buffer, sizeof(buffer) - 1);
        if (read_bytes < 0) {
            std::cerr << "Erro ao ler resposta: " << strerror(errno) << std::endl;
            return "";
        }
        buffer[read_bytes] = '\0';
        return std::string(buffer);
    }
};

class TekVISA {
public:
    Mailman mailman;

    // Construtor: Inicializa o objeto mailman
    TekVISA() {
        string idn_cmd = "*IDN?\n";
        mailman.Send(idn_cmd);
        string idn_response = mailman.Receive();
        std::cout << "Identificação do instrumento: " << idn_response << std::endl;
    }

    // MÉTODO PARA DEFINIR O CANAL ATUAL
    void SetChannel(const string& channel) {
        mailman.Send("DATA:SOURCE " + channel);
        mailman.Send("MEASU:IMM:SOURCE " + channel);
    }

    // MÉTODO PARA CONFIGURAR MEDIÇÕES
    void SetMeasurement(const string& channel) {
        // Usa um laço para evitar repetição de código
        string measurements[] = {"NWIDTH", "FALL", "RISE", "PK2PK"};

        for (int i = 0; i < 4; ++i) {
            string cmd1 = "MEASUREMENT:MEAS" + std::to_string(i + 1) + ":SOURCE " + channel;
            string cmd2 = "MEASUREMENT:MEAS" + std::to_string(i + 1) + ":TYPE " + measurements[i];
            mailman.Send(cmd1);
            mailman.Send(cmd2);
        }
    }
};

int main() {
    TekVISA scope;
    scope.SetChannel("CH1");
    scope.SetMeasurement("CH1");
    return 0;
}