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

#include "Core/TekVISA.h" // A classe que criamos anteriormente
#include "Core/Tools.h" // A classe que criamos anteriormente
#include "Core/Mailman.h" // A classe que criamos anteriormente
#include "Acquisition.h" // A classe que criamos anteriormente
#include <iostream>
#include <string>
#include <csignal> // Para capturar o sinal de Ctrl+C e parar os loops de aquisição
#include <vector>
#include <fstream>
#include <chrono>   // Para medir o tempo e obter o timestamp
#include <thread>   // Para std::this_thread::sleep_for
#include <iomanip>  // Para formatar a data
#include <ctime>    // Para std::time_t, std::localtime
#include <filesystem> // Para criar diretórios (requer C++17)
#include <atomic>  // For atomic flag


// Função para obter o timestamp atual formatado
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time);
    
    std::stringstream ss;
    ss << std::put_time(&local_tm, "%d/%m/%Y | %H:%M:%S");
    return ss.str();
}

std::atomic<bool> keepRunning(true);  // Atomic flag to control loop
// Function to handle user input
void checkInput() {
    std::string userInput;
    while (keepRunning) {
        std::getline(std::cin, userInput);  // Wait for user input
        if (userInput == "stop") {
            keepRunning = false;  // Stop the loop if "stop" is typed
        }
    }
}

void signalHandler(int signum) {
    keepRunning = false;
}

int main()
{
    // --- 1. Inicialização de Variáveis ---
    //public const string directory = "C:/Users/projetoMCA/Desktop/kauaWorkspace/projetoComunicacao/EXPERIMENTOS_TESTE/";
    const std::string risesArq = "rises.txt";
    const std::string ampArq = "amplitude.txt";
    const std::string ampXrisesArq = "ampXrises.txt";
    const std::string error = "9.9E37";
    int countEvents = 0;
    std::vector<std::string> measurements {"NWIDTH", "FALL", "RISE", "PK2PK"};
    std::vector<std::string> units {};
    
    //Pre-set values
    std::string experimentName = "teste";
    std::vector<std::string> channels = {"CH1"};
    std::string acquisitionMode = "1";
    int maxEvents = 20;
    
    try {
        // --- 2. Conexão e Configuração Inicial ---
        std::cout << "Conectando ao osciloscopio..." << std::endl;
        Instrument::Mailman mail; // only for tests !!!!!!!!! CHANGE THIS !!!!!!!!!
        Instrument::TekVISA Tv;
        Acquisition tools;

        std::cout << "Configurations: ";
        // Tv.SetChannel(channel);
        // Tv.SetMeasurement(channel);
        if (Tv.testConnection()){
            std::cout << "Conectado com sucesso!" << std::endl;
        }

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
            int mode;
            std::cout << "\nSelecione o modo de aquisição: ";
            std::cout << "\n    1. Aquisição Singular (1 canal apenas); ";
            std::cout << "\n    2. Aquisição Dupla (2 canais simultâneos).";
            std::cout << "\n>";
            std::cin >> mode;
            if (std::cin.fail() || mode < 1 || mode > 2) {
                std::cout << "Entrada invalida. Por favor, insira um numero entre 1 e 4." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                acquisitionMode = std::to_string(mode);
                break;
            }
        }
        
        if (acquisitionMode == "1"){
            while (true) {
                int ch_num;
                std::cout << "\nDigite o canal desejado (1-4): ";
                std::cin >> ch_num;
                if (std::cin.fail() || ch_num < 1 || ch_num > 4) {
                    std::cout << "Entrada invalida. Por favor, insira um numero entre 1 e 4." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                } else {
                    channels = {"CH" + std::to_string(ch_num)};
                    break;
                }
            }
        }
        else {
            while (true) {
                int ch_num;
                std::cout << "\nDigite o PRIMEIRO canal desejado (1-4): ";
                std::cin >> ch_num;
                if (std::cin.fail() || ch_num < 1 || ch_num > 4) {
                    std::cout << "Entrada invalida. Por favor, insira um numero entre 1 e 4." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                } else {
                    channels = {"CH" + std::to_string(ch_num)};
                    break;
                }
            }
            while (true) {
                int ch_num;
                std::cout << "\nDigite o SEGUNDO canal desejado (1-4): ";
                std::cin >> ch_num;
                if (std::cin.fail() || ch_num < 1 || ch_num > 4) {
                    std::cout << "Entrada invalida. Por favor, insira um numero entre 1 e 4." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                } else {
                    channels.push_back("CH" + std::to_string(ch_num));
                    break;
                }
            }
            std::cout << "\nCanais escolhidos: ";
            std::cout << "\n" + channels[0];
            std::cout << "\n" + channels[1];
        }
        
        
        // Start a separate thread to handle input
        std::thread inputThread(checkInput);
        // Define o tipo e o comando para capturar as medidas
        mail.Send("*DDT #274MEASU:IMM:TYPE NWIDTH;VAL?;TYPE FALL;VAL?;TYPE RISE;VAL?; TYPE PK2PK; VAL?");
        // Get Measurements UNITS
        units = Tv.GetIMMMeasurementsUnits(measurements);
        // Locks oscilloscope to prevent external interferences
        mail.Send("LOCK ALL");

        signal(SIGINT, signalHandler);
        if(acquisitionMode == "1"){
            // Assegura que os canais estejam visíveis no osciloscópio
            Tv.DisplayChannel(channels[0], true);

            // Criando cabeçalhos para as colunas
            dataFile << "Acquisition Time [days]" << "\t"
            << "Negative Width [" + units[0] + "]" << "\t" << "Fall Time [" + units[1] + "]" << "\t"
            << "Rise Time [" + units[2] + "]" << "\t" << "Peak-to-Peak [" + units[3] + "]" << std::endl;
            // Configura as medições no osciloscópio
            Tv.SetChannel(channels[0]);
            Tv.SetMeasurementsMEAS(channels[0], measurements);
            std::cout << "Canal " << channels[0] << " e medicoes configuradas." << std::endl;
            
            
            // --- 4. Loop Principal de Aquisição de Eventos --- //

            std::cout << "\n--- Iniciando aquisicao de " << maxEvents << " eventos ---" << std::endl;
            // Definição da query para measuIMM
            //std::string IMMquery = Tv.GenerateIMMquery({"NWIDTH", "FALL", "RISE", "PK2PK"});
            
            // Método para aquisição de medidas e salvamento dos dados
            tools.singleAcquisition(keepRunning, countEvents, maxEvents, dataFile);
            
        }
        else if (acquisitionMode == "2")
        {
            // Assegura que os canais estejam visíveis no osciloscópio
            Tv.DisplayChannel(channels[0], true);
            Tv.DisplayChannel(channels[1], true);

            // Criando cabeçalhos para as colunas
            dataFile << "Acquisition Time [days]" << "\t"
            << "Negative Width [" + units[0] + "]" << "\t" << "Fall Time [" + units[1] + "]" << "\t"
            << "Rise Time [" + units[2] + "]" << "\t" << "Peak-to-Peak [" + units[3] + "]" << "\t"
            << "Acquisition Time [days]" << "\t"
            << "Negative Width [" + units[0] + "]" << "\t" << "Fall Time [" + units[1] + "]" << "\t"
            << "Rise Time [" + units[2] + "]" << "\t" << "Peak-to-Peak [" + units[3] + "]" << std::endl;
            // Configura as medições no osciloscópio
            //Tv.SetChannel(channels[0]);
            //Tv.SetMeasurement(channel[0]); //por enquanto measurements está hard-coded
            //std::cout << "Canal " << channel << " e medicoes configuradas." << std::endl;
            
    
            // --- 4. Loop Principal de Aquisição de Eventos --- //
            std::cout << "\n--- Iniciando aquisicao de " << maxEvents << " eventos ---" << std::endl;
            // Definição da query para measuIMM
            //std::string IMMquery = Tv.GenerateIMMquery({"NWIDTH", "FALL", "RISE", "PK2PK"});
    
            // Método para aquisição de medidas e salvamento dos dados
            tools.dualAcquisition(keepRunning, countEvents, maxEvents, dataFile, channels);

        }
        else {
            std::cout << "Modo de aquisição inválido." << std::endl;
        }
        
        inputThread.join();  // Wait for input thread to finish
        std::cout << "Loop stopped." << std::endl;
        
        dataFile.close();
        std::cout << "\n--- Aquisicao concluida ---" << std::endl;

        // Unlocks oscilloscope to allow manipulation
        mail.Send("LOCK NONE");


    } catch (const std::runtime_error& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }


    return 0;
}