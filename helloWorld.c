/*
CÓDIGO HELLO WORLD FEITO PELO GPT

gcc -o osciloscopio helloWorld.c -lusb-1.0
./osciloscopio

expected: TEKTRONIX,TDS2024C,C010101,CF:91.1CT FV:v22.03

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID  0x0699
#define PRODUCT_ID 0x03a6

#define ENDPOINT_OUT 0x06  // geralmente para enviar, estava 0x01
#define ENDPOINT_IN  0x87  // geralmente para ler, estava 0x81 | 0x85

int main() {
    libusb_context *ctx = NULL;
    libusb_device_handle *dev_handle = NULL;
    int r;

    r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "Erro ao inicializar libusb: %s\n", libusb_error_name(r));
        return 1;
    }

    dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (dev_handle == NULL) {
        fprintf(stderr, "Dispositivo não encontrado\n");
        libusb_exit(ctx);
        return 1;
    }

    // Pode ser necessário separar o kernel
    libusb_detach_kernel_driver(dev_handle, 0);
    libusb_claim_interface(dev_handle, 0);

    // Enviar comando SCPI "*IDN?\n"
    char *scpi_cmd = "*IDN?\n";
    int transferred;
    r = libusb_bulk_transfer(dev_handle, ENDPOINT_OUT, (unsigned char *)scpi_cmd, strlen(scpi_cmd), &transferred, 10000);
    if (r == 0) {
        printf("Comando enviado (%d bytes)\n", transferred);
    } else {
        fprintf(stderr, "Erro ao enviar comando: %s\n", libusb_error_name(r));
    }

    // Ler resposta
    unsigned char buffer[512] = {0};
    r = libusb_bulk_transfer(dev_handle, ENDPOINT_IN, buffer, sizeof(buffer), &transferred, 10000);
    if (r == 0) {
        buffer[transferred] = '\0';
        printf("Resposta: %s\n", buffer);
    } else {
        fprintf(stderr, "Erro ao ler resposta: %s\n", libusb_error_name(r));
    }

    // Encerrar
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx);

    return 0;
}
