/*
 sudo echo -e "*IDN?" | sudo tee /dev/usbtmc0; sudo cat /dev/usbtmc0
*/
#include "Core/TekVISA.h" // A classe que criamos anteriormente
#include "Core/Mailman.h" // A classe Mailman separada
#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <list>
#include <vector>
#include <sstream>

namespace Instrument {

    Mailman mail;

    // Função helper para dividir strings, agora como um método privado.
    std::vector<std::string> TekVISA::split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    // Construtor: Inicializa o objeto mail
    TekVISA::TekVISA() {
        /* SOMETHING... */
    }

    bool TekVISA::testConnection() {
        std::string idn_response = mail.Query("*IDN?");
        if (idn_response.size() > 0){
            std::cout << "Identificação do instrumento: " << idn_response << std::endl;
            return true;
        }
        return false;
    }

    // MÉTODO PARA DEFINIR O CANAL ATUAL
    void TekVISA::SetChannel(const std::string& channel) {
        mail.Send("DATA:SOURCE " + channel);
        mail.Send("MEASU:IMM:SOURCE " + channel);
    }

    // MÉTODO PARA CONFIGURAR MEDIÇÕES
    void TekVISA::SetMeasurementsMEAS(const std::string channel, std::vector<std::string>& measurements) {
        int i = 0;
        for (std::string measurement : measurements) {

            // Concatenação de string em C++ usando o operador '+'
            std::string cmd_source = "MEASUREMENT:MEAS" + std::to_string(i + 1) + ":SOURCE " + channel;
            std::string cmd_type = "MEASUREMENT:MEAS" + std::to_string(i + 1) + ":TYPE " + measurement;
            mail.Send(cmd_source);
            mail.Send(cmd_type);
            i = i + 1;
        }
    }

    void TekVISA::Run()
    {
        mail.Send("ACQUIRE:STATE ON;");
    }

    void TekVISA::DisplayChannel(std::string channel, bool on) {
        std::string command = "SELECT:" + channel + (on ? "ON" : "OFF"); // std::to_string caso queira usar o parâmetro channel como int
        mail.Send(command);
    }
    
    std::vector<std::string> TekVISA::GetMeasurementsIMM(const std::vector<std::string>& measurements) {
        std::vector<std::string> results;
        for (const auto& meas_type : measurements) {

            mail.Send("MEASU:IMM:TYPE " + meas_type);
            
            std::string meas_value = mail.Query("MEASU:IMM:VAL?");

            /* Reduz a eficiência na resposta do método, pois verifica o ESR a cada iteração.
                Para o processo de aquisição, basta verificar uma vez após ter feitos todas as 
                medidas necessárias.
            */
            std::string esr = mail.Query("*ESR?");

            // O ESR (Event Status Register) 16 pode indicar um erro de sintaxe.
            // É uma boa prática verificar.
            if (esr.find("16") == std::string::npos) {
                results.push_back(meas_value);
            } else {
                std::cerr << "Aviso: Erro de comando ao medir " << meas_type << std::endl;
            }
        }
        return results;
    }

    std::string TekVISA::GenerateIMMquery(const std::vector<std::string>& measurements) {
        std::string variavel = "";
        int count = 0;
        for (const auto& meas_type : measurements) {
            if (count > 0) {
                variavel += ";:";
            }
            variavel += "MEASU:IMM:TYPE " + meas_type;
            variavel += ";:MEASU:IMM:VAL?";
            count++;
        }
        return variavel;
    }

    std::vector<std::string> TekVISA::GetDataIMM(std::string measu_string) {
        std::string response = mail.Query(measu_string);
        // Os valores retornam separados por ';', então o split deve usar esse delimitador.
        return split(response, ';');
    }

    std::vector<std::string> TekVISA::GetDataTRG() {
        mail.Send("*TRG");
        std::string response = mail.Receive();
        // Os valores retornam separados por ';', então o split deve usar esse delimitador.
        return split(response, ';');
    }

    std::vector<std::string> TekVISA::GetDataTRGCH(std::string channel) {
        mail.Send("MEASU:IMM:SOU " + channel + ";*TRG");
        std::string response = mail.Receive();
        // Os valores retornam separados por ';', então o split deve usar esse delimitador.
        return split(response, ';');
    }

    std::vector<std::string> TekVISA::GetData() {
        std::string response = mail.Query("MEASU:MEAS1:VAL?;:MEASU:MEAS2:VAL?;:MEASU:MEAS3:VAL?;:MEASU:MEAS4:VAL?");
        // Os valores retornam separados por ';', então o split deve usar esse delimitador.
        return split(response, ';');
    }

    std::vector<std::string> TekVISA::GetIMMMeasurementsUnits (std::vector<std::string>& measurements) {
        std::string meas_string = "";
        std::string IMMunits {};
        int count = 0;
        int i = 0;
        while (IMMunits.size() < measurements.size() && i < 5)
        { 
            for (const auto& meas_type : measurements) {
                if (count > 0) {
                    meas_string += ";:";
                }
                meas_string += "MEASUREMENT:IMMED:TYPE " + meas_type;
                meas_string += ";:MEASUREMENT:IMMED:UNITS?";
                count++;
            }
            IMMunits = mail.Query(meas_string);
            if (IMMunits.size() == measurements.size()){
                break;
            }
            i++;
        }
        if (IMMunits.size() < measurements.size())
        {
            std::cerr << "ERRO AO ADQUIRIR AS UNIDADES DE MEDIDA PARA MEASU:IMMED..." << std::endl;
        }
        return split(IMMunits, ';');
    }
    
    void TekVISA::PrintMeasurementsRaw() {
        std::string response = mail.Query("MEASU:MEAS1:VAL?;:MEASU:MEAS2:VAL?;:MEASU:MEAS3:VAL?;:MEASU:MEAS4:VAL?");
        std::cout << "\n--- Pegou ---\n";
        
        std::istringstream ss(response);
        std::string medida;
        
        while (std::getline(ss, medida, ';')) {
            std::cout << medida << std::endl;
        }
    }

    /*
    void Configure(const OscilloscopeConfigs& configs) {
        mail.Send(configs.DataFormatSet);
        mail.Send(configs.AcquireSet);
        mail.Send(configs.TriggerSet);
        mail.Send(configs.VisualizationSet);
    }
    */

    // Aguarda a conclusão da operação pendente.
    bool TekVISA::WaitData() {
        // O loop garante que esperamos o tempo que for necessário.
        while (true) {
            std::string opc_response = mail.Query("*OPC?");
            if (opc_response == "1") {
                return true; // Corrigido para retornar bool
            }
            usleep(50000); // Espera 50ms antes de perguntar de novo
        }
        return false; // Nunca deve chegar aqui no loop infinito
    }

    // Aguarda a conclusão da operação pendente.
    bool TekVISA::esr() {
        // O loop garante que esperamos o tempo que for necessário.
        std::string opc_response = mail.Query("*ESR?");
        if (opc_response == "16") {
            return true; // Há erros de medida
        }
        return false; // Sem erros asociados à medida
    }

    /*
    vector<string> GetData() {
        string all = mail.Query("MEASU:MEAS1:VAL?;:MEASU:MEAS2:VAL?;:MEASU:MEAS3:VAL?;:MEASU:MEAS4:VAL?");
        return split(all, ';');
    }
    */
    std::vector<std::string> TekVISA::GetTriggerConf() {
        std::string val = mail.Query("TRIG:MAI:MOD?;TYPE?;LEVEL?;VIDEO:SOURCE?;:TRIG:MAI:EDGE:SLOPE?;COUP?;");
        return split(val, ';');
    }

    std::vector<std::string> TekVISA::GetVisualizationConf() {
        std::string val = mail.Query("CH1:SCA?;POS?;:HOR:SCA?;POS?");
        return split(val, ';');
    }

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

}