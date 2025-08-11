#include "Core/Core.h"
#include "App/Source/classes/TekVISA/TekVISA.cpp"

#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

int main()
{

	TekVISA Tv;

	const std::string device = "/dev/usbtmc0";
	const std::string cmd = "*IDN?\n";
	char buffer[256] = {0};//Pode dar B.O
	int fd = 0;

	Core::PrintHelloWorld();

	
	fd = Tv.send(fd, cmd, device);

	fd = Tv.recive(fd);


}