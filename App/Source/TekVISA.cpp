
using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <list>
#include <sstream>
#include <boost>

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
    int Send(const string& cmd) {
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
    string Receive() {
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

    // Método para enviar e receber resposta
    string Query(const string& cmd) {
        Send(cmd);
        usleep(100000);
        return Receive();
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
        cout << "Identificação do instrumento: " << idn_response << endl;
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

    void Run()
    {
        mailman.Send("ACQUIRE:STATE ON;");
    }
    
    list<string> GetMeasurementsIMM(list<string> masurements)
    {
        string meas;
        string esr;
        list<string> all;
        for (int i = 0; i < masurements.Count(); i++)
        {
            mailman.Send("MEASU:IMM:TYPE {masurements[i]}");
            mailman.Send("MEASU:IMM:VAL?");
            meas = mailman.Receive();

            mailman.Send("*ESR?");
            esr = mailman.Receive();

            if (esr != "16")
            {
                all.Add(meas);
            }
        }
        return all;
    }

    list<string> GetData()
    {
        string all;
        list<string> data;
        bool status;
        mailman.Send("MEASU:MEAS1:VAL?;:MEASU:MEAS2:VAL?;:MEASU:MEAS3:VAL?;:MEASU:MEAS4:VAL?");
        istringstream iss(mailman.Receive());
        iss >> status;

        if (!status)
            cout << "Erro ao recuperar dados";

        split(data, all, is_any_of(","));
        return data;
    }

    void TekVISA::Configure(const OscilloscopeConfigs& configs) {
        mailman.Send(configs.DataFormatSet);
        mailman.Send(configs.AcquireSet);
        mailman.Send(configs.TriggerSet);
        mailman.Send(configs.VisualizationSet);
    }

    void TekVISA::SetChannel(const string& channel) {
    mailman.Send("DATA:SOURCE " + channel);
    mailman.Send("MEASU:IMM:SOURCE " + channel);
    }

    void TekVISA::SetMeasurement(const vector<string>& measurements, const std::string& channel) {
        for (size_t i = 0; i < measurements.size(); ++i) {
            mailman.Send("MEASUREMENT:MEAS" + to_string(i + 1) + ":SOURCE " + channel);
            mailman.Send("MEASUREMENT:MEAS" + to_string(i + 1) + ":TYPE " + measurements[i]);
        }
    }

    void TekVISA::Run() {
        mailman.Send("ACQUIRE:STATE ON;");
    }

    bool TekVISA::WaitData() {
        string stateTrigger = mailman.Query("*OPC?");
        return stateTrigger == "1";
    }

    vector<string> TekVISA::GetMeasurementsIMM(const vector<string>& measurements) {
        vector<string> all;
        for (const auto& meas_type : measurements) {
            mailman.Send("MEASU:IMM:TYPE " + meas_type);
            string meas = mailman.Query("MEASU:IMM:VAL?");
            string esr = mailman.Query("*ESR?");
            if (esr != "16") { // Verifica se não houve erro
                all.push_back(meas);
            }
        }
        return all;
    }

    vector<string> TekVISA::GetData() {
        string all = mailman.Query("MEASU:MEAS1:VAL?;:MEASU:MEAS2:VAL?;:MEASU:MEAS3:VAL?;:MEASU:MEAS4:VAL?");
        return split(all, ';');
    }

    vector<string> TekVISA::GetTriggerConf() {
        string val = mailman.Query("TRIG:MAI:MOD?;TYPE?;LEVEL?;VIDEO:SOURCE?;:TRIG:MAI:EDGE:SLOPE?;COUP?;");
        return split(val, ';');
    }

    vector<string> TekVISA::GetVisualizationConf() {
        std::string val = mailman.Query("CH1:SCA?;POS?;:HOR:SCA?;POS?");
        return split(val, ';');
    }

    // Implementação da função auxiliar split
    vector<string> TekVISA::split(const string& s, char delimiter) {
        vector<std::string> tokens;
        string token;
        istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }




};

/*
#include "TekVISA.h"
#include <iostream>
#include <sstream>
#include <unistd.h> // Para usleep

TekVISA::TekVISA(const std::string& devicePath) : devicePath_(devicePath) {}

TekVISA::~TekVISA() {
    if (deviceFile_.is_open()) {
        deviceFile_.close();
    }
}

bool TekVISA::Connect() {
    // Abre o arquivo do dispositivo para leitura e escrita
    deviceFile_.open(devicePath_, std::ios::in | std::ios::out);
    if (!deviceFile_.is_open()) {
        throw std::runtime_error("Erro: Nao foi possivel conectar com o osciloscopio em " + devicePath_);
    }
    return true;
}

void TekVISA::Write(const std::string& command) {
    if (!deviceFile_.is_open()) {
        throw std::runtime_error("Dispositivo nao esta conectado.");
    }
    deviceFile_ << command << std::endl;
}

std::string TekVISA::Read() {
    if (!deviceFile_.is_open()) {
        throw std::runtime_error("Dispositivo nao esta conectado.");
    }
    std::string response;
    std::getline(deviceFile_, response);
    // Remove o caractere de retorno de carro '\r' que alguns dispositivos enviam
    if (!response.empty() && response.back() == '\r') {
        response.pop_back();
    }
    return response;
}

std::string TekVISA::Query(const std::string& command) {
    Write(command);
    usleep(100000); // Pequena pausa (100ms) para o dispositivo processar
    return Read();
}

std::string TekVISA::GetID() {
    return Query("*IDN?");
}

void TekVISA::Configure(const OscilloscopeConfigs& configs) {
    Write(configs.DataFormatSet);
    Write(configs.AcquireSet);
    Write(configs.TriggerSet);
    Write(configs.VisualizationSet);
}

void TekVISA::SetChannel(const std::string& channel) {
    Write("DATA:SOURCE " + channel);
    Write("MEASU:IMM:SOURCE " + channel);
}

void TekVISA::SetMeasurement(const std::vector<std::string>& measurements, const std::string& channel) {
    for (size_t i = 0; i < measurements.size(); ++i) {
        Write("MEASUREMENT:MEAS" + std::to_string(i + 1) + ":SOURCE " + channel);
        Write("MEASUREMENT:MEAS" + std::to_string(i + 1) + ":TYPE " + measurements[i]);
    }
}

void TekVISA::Run() {
    Write("ACQUIRE:STATE ON;");
}

bool TekVISA::WaitData() {
    std::string stateTrigger = Query("*OPC?");
    return stateTrigger == "1";
}

std::vector<std::string> TekVISA::GetMeasurementsIMM(const std::vector<std::string>& measurements) {
    std::vector<std::string> all;
    for (const auto& meas_type : measurements) {
        Write("MEASU:IMM:TYPE " + meas_type);
        std::string meas = Query("MEASU:IMM:VAL?");
        std::string esr = Query("*ESR?");
        if (esr != "16") { // Verifica se não houve erro
            all.push_back(meas);
        }
    }
    return all;
}

std::vector<std::string> TekVISA::GetData() {
    std::string all = Query("MEASU:MEAS1:VAL?;:MEASU:MEAS2:VAL?;:MEASU:MEAS3:VAL?;:MEASU:MEAS4:VAL?");
    return split(all, ';');
}

std::vector<std::string> TekVISA::GetTriggerConf() {
    std::string val = Query("TRIG:MAI:MOD?;TYPE?;LEVEL?;VIDEO:SOURCE?;:TRIG:MAI:EDGE:SLOPE?;COUP?;");
    return split(val, ';');
}

std::vector<std::string> TekVISA::GetVisualizationConf() {
    std::string val = Query("CH1:SCA?;POS?;:HOR:SCA?;POS?");
    return split(val, ';');
}

// Implementação da função auxiliar split
std::vector<std::string> TekVISA::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

*/