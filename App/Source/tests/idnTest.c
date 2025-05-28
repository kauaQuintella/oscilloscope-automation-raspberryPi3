/*
CÓDIGO HELLO WORLD FEITO PELO GPT

gcc -o exe/idnC test/idnTest.c
./idnC

expected: TEKTRONIX,TDS 2024C,C042765,CF:91.1CT FV:v24.26

*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    const char* device = "/dev/usbtmc0";
    const char* cmd = "*IDN?\n";
    char buffer[256] = {0};
    
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("Erro ao abrir dispositivo");
        return 1;
    }

    if (write(fd, cmd, strlen(cmd)) < 0) {
        perror("Erro ao enviar comando");
        close(fd);
        return 1;
    }

    //usleep(100000); // Espera 100ms

    int n = read(fd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("Erro ao ler resposta");
        close(fd);
        return 1;
    }

    buffer[n] = '\0';
    printf("Resposta: %s\n", buffer);
    close(fd);
    return 0;
}