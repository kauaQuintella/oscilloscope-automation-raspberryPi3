#ifndef MAILMAN_H
#define MAILMAN_H

#include <string>

/**
    * @class Mailman
    * @brief Gerencia a comunicação de baixo nível com o Osciloscópio via file descriptor.
    * Utiliza o padrão RAII para garantir que o dispositivo seja aberto e fechado corretamente.
    */
namespace Instrument{
    class Mailman {
    public:
        Mailman();
        ~Mailman();
        Mailman(const Mailman&) = delete;
        Mailman& operator=(const Mailman&) = delete;

        void Send(const std::string& cmd);
        std::string Receive();
        std::string Query(const std::string& cmd);

    private:
        const std::string device = "/dev/usbtmc0";
        int fd = -1;
    };
}

#endif
