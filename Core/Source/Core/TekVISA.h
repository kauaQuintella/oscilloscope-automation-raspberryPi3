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

class TekVISA {
public:
    // Construtor: Prepara a classe para a conexão
    TekVISA(const std::string& devicePath = "/dev/usbtmc0");

    // Destrutor: Garante que o arquivo do dispositivo seja fechado
    ~TekVISA();

    // Conecta (abre o arquivo do dispositivo)
    bool Connect();

    // Envia um comando para o osciloscópio
    void Write(const std::string& command);

    // Lê uma resposta do osciloscópio
    std::string Read();

    // Envia um comando e espera uma resposta
    std::string Query(const std::string& command);

    // Obtém o ID do dispositivo (*IDN?)
    std::string GetID();

    // Configura o osciloscópio com base na estrutura de configurações
    void Configure(const OscilloscopeConfigs& configs);

    // Define o canal de medição
    void SetChannel(const std::string& channel);

    // Configura os tipos de medição
    void SetMeasurement(const std::vector<std::string>& measurements, const std::string& channel);

    // Inicia a aquisição de dados
    void Run();

    // Espera a operação ser concluída
    bool WaitData();
    
    // Obtém dados de medições pré-configuradas
    std::vector<std::string> GetData();
    
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

#endif // TEKVISA_H