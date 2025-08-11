#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

class TekVISA {
    public:
        const std::string device = "/dev/usbtmc0";
        char buffer[256] = {0};//Pode dar B.O

        int send (int fd,  std::string cmd ,  std::string device){

                device = "/dev/usbtmc0";
                cmd = "*IDN?\n";

                fd = open(device.c_str(), O_RDWR);
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
                
                return fd;
        };

        int recive (int fd) {
            
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
        };
};