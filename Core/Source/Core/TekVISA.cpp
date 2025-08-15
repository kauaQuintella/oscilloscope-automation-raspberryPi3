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
        
        std::string cmd;
        int fd;

        Mailman(int fd, std::string cmd ){
            this->cmd = cmd;
            this->fd = fd;
        }

        int Send (std::string cmd , int fd){

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


        int Recive (int fd) {
            
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
}


class TekVISA {

    public:

        Mailman mailman;
        string cmd;
        int fd;

        TekVISA (){
            fd = 0;
            cmd = "*IDN?\n"; 
            mailman = Mailman (fd, cmd );//pode dar erro
        }

        // MÉTODO PARA DEFINIR O CANAL ATUAL
        void SetChannel(string channel)
        {
            mailman.Send("DATA:SOURCE "+channel);
            mailman.Send("MEASU:IMM:SOURCE "+channel);
            string measuda;
        }

        // MÉTODO PARA CONFIGURAR MEDIÇÕES
        void SetMeasurement(string channel)
        {
            //sem laço por enquanto
            int i = 1; 
            string measure = "NWIDTH";
            mailman.Send("MEASUREMENT:MEAS"+i+":SOURCE "+channel);
            mailman.Send("MEASUREMENT:MEAS"+i+":TYPE "+measure);
            
            i = 2; 
            measure = "FALL";
            mailman.Send("MEASUREMENT:MEAS"+i+":SOURCE "+channel);
            mailman.Send("MEASUREMENT:MEAS"+i+":TYPE "+measure);

            i = 3; 
            measure = "RISE";
            mailman.Send("MEASUREMENT:MEAS"+i+":SOURCE "+channel);
            mailman.Send("MEASUREMENT:MEAS"+i+":TYPE "+measure);
        
            i = 4; 
            measure = "PK2PK";
            mailman.Send("MEASUREMENT:MEAS"+i+":SOURCE "+channel);
            mailman.Send("MEASUREMENT:MEAS"+i+":TYPE "+measure);
        
        }



};