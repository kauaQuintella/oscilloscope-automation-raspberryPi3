# oscilloscope-automation-raspberryPi3

Projeto da Iniciação Tecnológica com o intuito de criar uma automação para o osciloscópio digital TDS 2024C e enviar os dados para uma cloud. Neste projeto, estamos usando a comunicação USBTMC para obter os querries do ociliscópio.

### Configuração de ambiente

Para criar o projeto, foi usado o template obtido no vídeo How to Properly Setup C++ Projects[1]. Para configuração do ambiente, é necessário executar o "Setup-Linux.sh" em "Scripts" que vai gerar o Makefile do nosso projeto através do Premake. Contudo, o binário premake5 que está no código não é compatível com a arquitetura do Raspberry Pi 3, já que ele é ARM. Para solucionar isso, foi necessário compilar o Premake no Raspberry executando os seguintes passos na pasta "./Vendor/Binaries/Premake":

        sudo apt update
        git clone --recursive https://github.com/premake/premake-core.git
        cd premake-core
        make -f Bootstrap.mak linux

Nesse projeto, houve-se o erro de falta da biblioteca de desenvolvimento de UUID e para isto, bastou fazer a instalação dela pelo comando:

        sudo apt install -y uuid-dev

Após isso, fez-se necessário substituir o diretório do premake5 para "Vendor/Binaries/Premake/Linux/premake-core/bin/release/premake5", que é o caminho relativo dele. No final, o Setup-Linux.sh ficou assim:

        #!/bin/bash

        pushd ..
        Vendor/Binaries/Premake/Linux/premake-core/bin/release/premake5 --cc=clang --file=Build.lua gmake2
        popd

E dessa forma, executando normalmente ao executar.

#### Entendendo a comunicação USBTMC
- O osciloscópio suporta o protocolo USBTMC (USB Test & Measurement Class).
- Ele aparece no sistema como um dispositivo de caractere em /dev/usbtmc0.
- Esse dispositivo aceita comandos SCPI, como *IDN?, e responde diretamente.

## Passo a passo para realização do teste

### Passo 1: Verificar suporte USBTMC

- Certifique-se de que o kernel do Linux tem suporte ao driver USBTMC (USB Test and Measurement Class).
- Para carregar o módulo manualmente, execute:

        sudo modprobe usbtmc

- Após conectar o osciloscópio via USB, verifique se o dispositivo foi criado:

        ls /dev/usbtmc*


### Passo 2: Ajustar permissões de acesso

- Por padrão, o dispositivo /dev/usbtmc0 é acessível apenas por root. Para permitir acesso sem sudo, crie uma regra udev:

        sudo nano /etc/udev/rules.d/99-usbtmc.rules

    Conteúdo do arquivo:

         SUBSYSTEM=="usb", ATTR{idVendor}=="0699", MODE="0666"

    Substitua "0699" pelo ID do fabricante, se necessário. Após salvar, aplique com:

        sudo udevadm control --reload
        sudo udevadm trigger

### Passo 3: Testar comandos SCPI manualmente

- Com o dispositivo conectado, envie comandos SCPI diretamente do terminal:

        echo "*IDN?" | sudo tee /dev/usbtmc0
        sudo cat /dev/usbtmc0

    O comando *IDN? solicita a identificação do instrumento. A resposta deve conter modelo e fabricante.

## Referências

[1] https://www.youtube.com/watch?v=5glH8dGoeCA