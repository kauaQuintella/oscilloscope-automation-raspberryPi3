/*
CÓDIGO HELLO WORLD FEITO PELO GPT

g++ -o exe/app test/App.cpp
./exe/app

expected: TEKTRONIX,TDS 2024C,C042765,CF:91.1CT FV:v24.26

*/

using namespace std;

#include "Core/Core.h"
#include "Core/TekVISA.cpp"

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

	Core::PrintHelloWorld();
	
	cout << "Configurations: ";
	fd = Tv.SetChannel(channel);
	fd = Tv.SetMeasurement(channel);

    
}