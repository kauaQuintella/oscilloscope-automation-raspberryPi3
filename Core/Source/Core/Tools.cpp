// Tools.cpp
#include "Core/TekVISA.h" // A classe que criamos anteriormente
#include "Core/Tools.h" // A classe que criamos anteriormente
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


const double JULIAN_EPOCH = 2440588.0;  // Julian date for Unix Epoch (1970-01-01)
const double SECONDS_IN_A_DAY = 86400.0;  // Number of seconds in one day


// Function to convert a Unix timestamp to Julian Date
double Tools::getJulianDate(time_t timestamp) {
    return JULIAN_EPOCH + (timestamp / SECONDS_IN_A_DAY);
}

