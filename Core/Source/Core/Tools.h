// Tools.h
#ifndef TOOLS_H
#define TOOLS_H

#include <ctime>
#include <fstream>
#include "TekVISA.h"

class Tools {
public:
    // Method to get Julian Date from a Unix timestamp
    static double getJulianDate(time_t timestamp);
    //void singleAcquisition(bool condition, int countEvents, int maxEvents, std::ofstream& dataFile, Instrument::TekVISA& Tv);
};

#endif // TOOLS_H
