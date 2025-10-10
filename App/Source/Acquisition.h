// Acquisition.h
#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <ctime>
#include <fstream>
#include <atomic>
#include "Core/TekVISA.h"

class Acquisition {
public:
    void singleAcquisition(std::atomic<bool>& condition, int countEvents, int maxEvents, std::ofstream& dataFile);
    void dualAcquisition(std::atomic<bool>& condition, int countEvents, int maxEvents, std::ofstream& dataFile, std::vector<std::string>& channels);
};

#endif // ACQUISITION_H
