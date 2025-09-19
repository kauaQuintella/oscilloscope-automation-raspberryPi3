/*
CÓDIGO HELLO WORLD FEITO PELO GPT

g++ -o exe/app App.cpp
./exe/app

expected: TEKTRONIX,TDS 2024C,C042765,CF:91.1CT FV:v24.26



using namespace std;

#include "TekVISA.cpp"

#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <vector>
#include <string>

int main()
{
	

	//public const string directory = "C:/Users/projetoMCA/Desktop/kauaWorkspace/projetoComunicacao/EXPERIMENTOS_TESTE/";
	const std::string risesArq = "rises.txt";
	const std::string ampArq = "amplitude.txt";
	const std::string ampXrisesArq = "ampXrises.txt";
	const std::string error = "9.9E37";

	const std::string device = "/dev/usbtmc0";
	const std::string cmd = "*IDN?\n";
	char buffer[256] = {0};//Pode dar B.O
	int fd = 0;

	TekVISA Tv;

	const std::string nameExperiment = "teste";
	const std::string channel="CH1";
	const int maxEvents = 20;
	
	cout << "Configurations: ";
	Tv.SetChannel(channel);
	Tv.SetMeasurement(channel);

    
}
*/

#include "Core/Core.h"
#include "Core/TekVISA.h" // A classe que criamos anteriormente
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>   // Para medir o tempo e obter o timestamp
#include <thread>   // Para std::this_thread::sleep_for
#include <iomanip>  // Para formatar a data
#include <ctime>    // Para std::time_t, std::localtime
#include <filesystem> // Para criar diretórios (requer C++17)

// Função para obter o timestamp atual formatado
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time);

    std::stringstream ss;
    ss << std::put_time(&local_tm, "%d/%m/%Y | %H:%M:%S");
    return ss.str();
}

int main()
{
    // --- 1. Inicialização de Variáveis ---
    TekVISA scope;
    const std::string errorValue = "9.9E+37"; // Valor de erro do osciloscópio
    std::vector<std::string> measurements = {"NWIDTH", "FALL", "RISE", "PK2PK"};
    int maxEvents = 0;
    int countEvents = 0;
    std::string experimentName;
    std::string channel;

    try {
        // --- 2. Conexão e Configuração Inicial ---
        std::cout << "Conectando ao osciloscopio..." << std::endl;
        scope.Connect();
        std::cout << "Conectado com sucesso ao: " << scope.GetID() << std::endl;

        // --- 3. Interação com o Usuário ---
        std::cout << "\nDigite o nome do experimento: ";
        std::cin >> experimentName;

        // Cria o diretório para o experimento
        std::filesystem::path experimentPath = std::filesystem::current_path() / "experiments" / experimentName;
        std::filesystem::create_directories(experimentPath);

        // Cria e abre o arquivo para salvar os dados
        std::ofstream dataFile(experimentPath / "data.txt");
        if (!dataFile.is_open()) {
            throw std::runtime_error("Nao foi possivel criar o arquivo de dados.");
        }
        std::cout << "Pasta e arquivo de dados criados em: " << experimentPath << std::endl;

        // Define o número máximo de eventos
        while (true) {
            std::cout << "\nDigite o maximo de eventos: ";
            std::cin >> maxEvents;
            if (std::cin.fail() || maxEvents <= 0) {
                std::cout << "Entrada invalida. Por favor, insira um numero inteiro positivo." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                break;
            }
        }
        
        // Seleção de Canal
        while (true) {
            int ch_num;
            std::cout << "\nDigite o canal desejado (1-4): ";
            std::cin >> ch_num;
             if (std::cin.fail() || ch_num < 1 || ch_num > 4) {
                std::cout << "Entrada invalida. Por favor, insira um numero entre 1 e 4." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             } else {
                channel = "CH" + std::to_string(ch_num);
                break;
             }
        }

        // Configura as medições no osciloscópio
        scope.SetChannel(channel);
        scope.SetMeasurement(measurements, channel);
        std::cout << "Canal " << channel << " e medicoes configuradas." << std::endl;


        // --- 4. Loop Principal de Aquisição de Eventos ---
        std::cout << "\n--- Iniciando aquisicao de " << maxEvents << " eventos ---" << std::endl;

        while (countEvents < maxEvents) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            scope.Run(); // Arma o osciloscópio para a próxima aquisição

            // Espera o osciloscópio ter dados prontos
            while (!scope.WaitData()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Pequena pausa para não sobrecarregar
            }

            std::vector<std::string> data = scope.GetData();

            // VERIFICA SE OCORREU O ERRO "9.9E37"
            bool hasError = false;
            for(const auto& val : data) {
                if(val.find("9.9E+37") != std::string::npos) {
                    hasError = true;
                    break;
                }
            }
            if (hasError) {
                std::cout << "Erro detectado (9.9E+37), descartando evento." << std::endl;
                continue; // Pula para a próxima iteração do loop
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;

            if (data.size() >= 4) {
                countEvents++;
                std::cout << "Evento: " << countEvents << "/" << maxEvents << " | Tempo de aquisicao: " << elapsedTime.count() << " ms" << std::endl;
                std::cout << "  Negative Width: " << data[0] << " | Fall Time: " << data[1] 
                          << " | Rise Time: " << data[2] << " | Peak-to-Peak: " << data[3] << std::endl;

                // Salva os dados no arquivo
                dataFile << getCurrentTimestamp() << "\t"
                         << data[0] << "\t" << data[1] << "\t"
                         << data[2] << "\t" << data[3] << std::endl;
            }
        }
        
        dataFile.close();
        std::cout << "\n--- Aquisicao concluida ---" << std::endl;


    } catch (const std::runtime_error& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}