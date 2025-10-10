// Acquisition.cpp
#include "Core/TekVISA.h" // A classe que criamos anteriormente
#include "Core/Tools.h" // A classe que criamos anteriormente
#include "Acquisition.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>   // Para medir o tempo e obter o timestamp
#include <thread>   // Para std::this_thread::sleep_for
#include <iomanip>  // Para formatar a data
#include <ctime>    // Para std::time_t, std::localtime
#include <filesystem> // Para criar diretórios (requer C++17)
#include <atomic>  // For atomic flag

Instrument::TekVISA Tv;

void Acquisition::singleAcquisition(std::atomic<bool>& condition, int countEvents, int maxEvents, std::ofstream& dataFile){
    while (condition)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
    
        Tv.Run(); // Arma o osciloscópio para a próxima aquisição
    
        // Espera o osciloscópio ter dados prontos
        while (!Tv.WaitData()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Pequena pausa para não sobrecarregar
            std::cout << "\n--- Esperou ---" << std::endl;
        }
        // MEASUREMENT MEAS
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //std::vector<std::string> data = Tv.GetData();
    
        // MEASUREMENT IMMED otimizado
        //std::vector<std::string> data = Tv.GetDataIMM(IMMquery);
    
        // MEASUREMENT IMMED não otimizado
        //std::vector<std::string> data = Tv.GetMeasurementsIMM({"NWIDTH", "FALL", "RISE", "PK2PK"});
    
        // DDT and TRG
        std::vector<std::string> data = Tv.GetDataTRG();
    
    
        // VERIFICA SE OCORREU O ERRO "9.9E37"
        bool hasError = false;
        if(Tv.esr()) {
            hasError = true;
        }
        for(const auto& val : data) {
            if(val.find("9.9E37") != std::string::npos) {
                hasError = true;
                break;
            }
        }
        if (hasError) {
            std::cout << "Erro de medida detectado, descartando evento..." << std::endl;
            continue; // Pula para a próxima iteração do loop
        }
    
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;
    
        if (data.size() >= 4) {
            countEvents++;
            std::cout << "Evento: " << countEvents << "/" << maxEvents << " | Tempo de aquisicao: " << elapsedTime.count() << " ms" << std::endl;
            std::cout << "  Negative Width: " << data[0] << " | Fall Time: " << data[1] 
                        << " | Rise Time: " << data[2] << " | Peak-to-Peak: " << data[3] << std::endl;
    
            // Get the current time as Unix timestamp
            time_t currentTimestamp = time(0);
            
            // Convert to Julian Date using the Tools class
            double julianDate = Tools::getJulianDate(currentTimestamp);
            
            // Print the Julian Date
            std::cout << std::fixed << std::setprecision(12);  // e.g., 6 digits after decimal
            std::cout << "Current Julian Date: " << julianDate << std::endl;
            // Salva os dados no arquivo
            /*dataFile << getCurrentTimestamp() << "\t"
                        << data[0] << "\t" << data[1] << "\t"
                        << data[2] << "\t" << data[3] << std::endl;*/
            dataFile << std::fixed << std::setprecision(12);
            dataFile << julianDate << "\t"
                        << data[0] << "\t" << data[1] << "\t"
                        << data[2] << "\t" << data[3] << std::endl;
        }
        else {
            std::cout << "Dados quebrados..." << std::endl;
        }
    }
    
}

void Acquisition::dualAcquisition(std::atomic<bool>& condition, int countEvents, int maxEvents, std::ofstream& dataFile, std::vector<std::string>& channels){
    while (condition)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
    
        Tv.Run(); // Arma o osciloscópio para a próxima aquisição
    
        // Espera o osciloscópio ter dados prontos
        while (!Tv.WaitData()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Pequena pausa para não sobrecarregar
            std::cout << "\n--- Esperou ---" << std::endl;
        }
        // MEASUREMENT MEAS
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //std::vector<std::string> data = Tv.GetData();
    
        // MEASUREMENT IMMED otimizado
        //std::vector<std::string> data = Tv.GetDataIMM(IMMquery);
    
        // MEASUREMENT IMMED não otimizado
        //std::vector<std::string> data = Tv.GetMeasurementsIMM({"NWIDTH", "FALL", "RISE", "PK2PK"});
    
        // DDT and TRG
        std::vector<std::string> data = Tv.GetDataTRGCH(channels[0]);
        std::vector<std::string> temp = Tv.GetDataTRGCH(channels[1]);
        data.insert(data.end(), temp.begin(), temp.end());
        /*for (int x = 0; x < data.size(); x++) {
            std::cout << data[x] << " "; // Output: 1 2 3 4
        }*/
    
        // VERIFICA SE OCORREU O ERRO "9.9E37"
        bool hasError = false;
        if(Tv.esr()) {
            hasError = true;
        }
        for(const auto& val : data) {
            if(val.find("9.9E37") != std::string::npos) {
                hasError = true;
                break;
            }
        }
        if (hasError) {
            std::cout << "Erro de medida detectado, descartando evento..." << std::endl;
            continue; // Pula para a próxima iteração do loop
        }
    
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;
    
        if (data.size() >= 8) {
            countEvents++;
            std::cout << "\n\nCANAL " + channels[0] << std::endl;
            std::cout << "Evento " + channels[0] + ": " << countEvents << "/" << maxEvents << " | Tempo de aquisicao: " << elapsedTime.count() << " ms" << std::endl;
            std::cout << "  Negative Width: " << data[0] << " | Fall Time: " << data[1] 
                        << " | Rise Time: " << data[2] << " | Peak-to-Peak: " << data[3] << std::endl;

            std::cout << "\nCANAL " + channels[1] << std::endl;
            std::cout << "Evento " + channels[1] + ": " << countEvents << "/" << maxEvents << " | Tempo de aquisicao: " << elapsedTime.count() << " ms" << std::endl;
            std::cout << "  Negative Width: " << data[4] << " | Fall Time: " << data[5] 
                        << " | Rise Time: " << data[6] << " | Peak-to-Peak: " << data[7] << std::endl;
    
            // Get the current time as Unix timestamp
            time_t currentTimestamp = time(0);
            
            // Convert to Julian Date using the Tools class
            double julianDate = Tools::getJulianDate(currentTimestamp);
            
            // Print the Julian Date
            std::cout << std::fixed << std::setprecision(12);  // e.g., 6 digits after decimal
            std::cout << "Current Julian Date: " << julianDate << std::endl;
            // Salva os dados no arquivo
            dataFile << std::fixed << std::setprecision(12);
            dataFile << julianDate << "\t"
                        << data[0] << "\t" << data[1] << "\t"
                        << data[2] << "\t" << data[3] << "\t"
                        << julianDate << "\t" << data[4] << "\t"
                        << data[5] << "\t" << data[6] << "\t"
                        << data[7] << std::endl;
        }
        else {
            std::cout << "Dados quebrados..." << std::endl;
        }
    }
    
}

