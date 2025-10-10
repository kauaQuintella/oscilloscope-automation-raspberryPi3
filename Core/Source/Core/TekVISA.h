#ifndef TEKVISA_H
#define TEKVISA_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

// Estrutura para manter as configurações do osciloscópio
struct OscilloscopeConfigs {
    std::string DataFormatSet;
    std::string AcquireSet;
    std::string TriggerSet;
    std::string VisualizationSet;
};

namespace Instrument {
    class TekVISA {
    public:
        // Construtor: Prepara a classe para a conexão
        TekVISA();
    
        // Destrutor: Garante que o arquivo do dispositivo seja fechado
        //~Mailman(); CLASS MAILMAN

        // Testa a conexão com o osciloscópio enviando a query *IDN?
        bool testConnection();
    
        // Envia um comando para o osciloscópio
        //void Send(const std::string& command);
        //void Send(const std::string& cmd);
    
    
        // Lê uma resposta do osciloscópio
        //std::string Receive();
    
        // Envia um comando e espera uma resposta
        std::string Query(const std::string& command);
    
        // Obtém o ID do dispositivo (*IDN?)
        std::string GetID();
    
        // Configura o osciloscópio com base na estrutura de configurações
        void Configure(const OscilloscopeConfigs& configs);
    
        // Define o canal de medição
        void SetChannel(const std::string& channel);
    
        // Configura os tipos de medição
        void SetMeasurementsMEAS(const std::string channel, std::vector<std::string>& measurements);

        // Aciona ou desativa a visualização dos canais
        void DisplayChannel(std::string channel, bool on);
    
        // Inicia a aquisição de dados
        void Run();
    
        // Espera a operação ser concluída
        bool WaitData();

        // Verifica por erros nas medições
        bool esr();

        // Gera a query para o MEASUREMENT:IMMED
        std::string GenerateIMMquery(const std::vector<std::string>& measurements);

        // Obtém dados de medições passadas no parâmetro e verifica por erros
        std::vector<std::string> GetMeasurementsIMM(const std::vector<std::string>& measurements);

        // Obtém dados de medições e apresenta sem tratamento
        void PrintMeasurementsRaw();

        // Apenas obtém os dados com base na query passada
        std::vector<std::string> GetDataIMM(std::string measu_string);
        
        // Obtém dados de medições pré-configuradas
        std::vector<std::string> GetData();

        // Obtém as medições pré-configuradas via *TRG
        std::vector<std::string> GetDataTRG();

        // Obtém as medições pré-configuradas via *TRG no canal Channel
        std::vector<std::string> GetDataTRGCH(std::string channel);

        // Obtém as unidades de medidas no MEASU:IMM
        std::vector<std::string> GetIMMMeasurementsUnits (std::vector<std::string>& measurements);
        
        // Obtém a configuração do Trigger
        std::vector<std::string> GetTriggerConf();
    
        // Obtém a configuração de visualização
        std::vector<std::string> GetVisualizationConf();
    
    private:
        std::string devicePath_;
        std::fstream deviceFile_; // Objeto para interagir com o arquivo do dispositivo
    
        // Função auxiliar para dividir strings
        static std::vector<std::string> split(const std::string& s, char delimiter);
    };
}

#endif // TEKVISA_H