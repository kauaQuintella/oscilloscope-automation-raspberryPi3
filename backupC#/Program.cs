using System;
using System.Threading;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Windows;

//using static System.Console;
using System.Diagnostics;

using comunicacaoOciloscopio.classes; //ACESSO À PASTA DE CLASSES


// NAMESPACE PRINCIPAL
namespace TekOscilloscopeCommunication
{ 
    class Program
    {
        //public const string directory = "C:/Users/projetoMCA/Desktop/kauaWorkspace/projetoComunicacao/EXPERIMENTOS_TESTE/";
        public const string risesArq = "rises.txt";
        public const string ampArq = "amplitude.txt";
        public const string ampXrisesArq = "ampXrises.txt";
        public const string error = "9.9E37";

        static void Main(string[] args)
        {
            // Veriáveis e estruturas para "DEV MODE"
            bool dev = false;
            string projectPath = "C:/Users/projetoMCA/Desktop/GitHub Desktop/oscilloscope-automation";
            List<string> measurements = new List<string> { "NWIDTH", "FALL", "RISE", "PK2PK" };


            // Inicialização de variáveis
            UserConfigs userConfigs = new UserConfigs(" ", " ", " ", 0, " ");
            string userConfigsPath = "./userConfigs.json";
            OscilloscopeConfigs oscilloscopeConfigs = new OscilloscopeConfigs(" ", " ", " ", " ");
            string oscilloscopeConfigsPath = "./userConfigs.json";
            DataAcquisition data = new DataAcquisition("", "", "", new List<string>(), "", "", "", "");
            TekVISA tekVISA = new TekVISA();
            var stopwatch = new Stopwatch();

            if (dev)
            {
                userConfigsPath = $"{projectPath}/userConfigs.json";
                oscilloscopeConfigsPath = projectPath;
            }

            var storageServiceUserConfigs = new JsonStorageService<UserConfigs>(userConfigsPath);
            var storageServiceOscilloscopeConfigs = new JsonStorageService<OscilloscopeConfigs>(oscilloscopeConfigsPath);
            int countEvents = 0;



            // BEGINNING FileManager directory SET
            FileManager fileManager = new FileManager("");
            storageServiceUserConfigs.FilePath = $"{projectPath}/userConfigs.json";
            userConfigs = storageServiceUserConfigs.Load();
            string directory = $"{userConfigs.Directory}";
            fileManager.directory = directory;
            // END directory FileManager SET



            //userConfigs.readSetupFile();
            /*
            DateTime now = DateTime.Now;
            double julianDate = data.ConverterParaDataJuliana(now);
            Console.WriteLine($"Data Juliana: {julianDate}")*/

            // Carregar o objeto UserConfig do JSON
            /*
            try
            {
                userConfigs = storageService.Load();
                Console.WriteLine($"Configuração carregada!");
            }
            catch (FileNotFoundException ex)
            {
                Console.WriteLine(ex.Message);
            }
            */

            //Console.WriteLine("Hello World!");


            // CRIAÇÃO DA PASTA
            Console.WriteLine("\nDigite o nome do experimento: ");
            String pathName = Console.ReadLine();
            fileManager.createFile($"/{pathName}");//CRIAÇÃO DE PASTA E ARQUIVOS

            /*
            string folderPath = @directory + userConfigs.Directory;
            System.IO.Directory.CreateDirectory(folderPath);

            data.RisePath = System.IO.Path.Combine(folderPath, risesArq);
            data.AmplitudePath = System.IO.Path.Combine(folderPath, ampArq);
            data.AmpXrisesPath = System.IO.Path.Combine(folderPath, ampXrisesArq);

            Console.WriteLine("\n\n\tPASTA CRIADA COM SUCESSO!\n");

            // CRIAÇÃO DE ARQUIVO
            string caminhoArquivo = @directory + userConfigs.Directory + "/aquisitions.txt";
            string horaAtual = "";
            string dataAtual = "";


            /*
            // Verificar se o arquivo existe, caso contrário, criar
            if (!File.Exists(caminhoArquivo))
            {
                File.Create(caminhoArquivo).Dispose();
            }
            */

            // MÁXIMO DE EVENTOS
            while (true)
            {
                Console.WriteLine("\nDigite o máximo de eventos: ");
                //string maxEventsInput = Console.ReadLine();
                string maxEventsInput = "100";

                if (int.TryParse(maxEventsInput, out int intMaxEvent))
                {
                    userConfigs.MaxEvents = intMaxEvent;
                    Console.WriteLine($"\n\n\tMÁXIMOS EVENTOS: {userConfigs.MaxEvents}\n");
                }
                else
                {
                    Console.WriteLine("Entrada inválida para o número máximo de eventos.");
                    continue;
                }
                break;
            }
            
            
            // SELEÇÃO DE CANAL
            while (true)
            {
                Console.WriteLine(@"Digite o canal almejado: 
1) CH1
2) CH2
3) CH3
4) CH4");
                userConfigs.Channel = Console.ReadLine();
                //userConfigs.Channel = "1";
                if (!int.TryParse(userConfigs.Channel, out int channelInt))
                {
                    Console.WriteLine($"Digite uma opção válida\n");
                    continue;
                }
                if (!(channelInt >= 1 && channelInt <= 4))
                {
                    Console.WriteLine($"Digite uma opção válida\n");
                    continue;
                }
                userConfigs.Channel = $"CH{userConfigs.Channel}";
                Console.WriteLine($"canal {userConfigs.Channel}");
                break;
            }
            

            storageServiceUserConfigs.FilePath = userConfigsPath;
            storageServiceUserConfigs.Save(userConfigs);

            // INICIALIZAÇÃO DO TEKVISA E CONEXÃO
            var resources = tekVISA.GetResources();
            if (resources.Count == 0)
            {
                Console.WriteLine("Nenhum osciloscópio detectado.");
                return;
            }

            userConfigs.Resource = resources[0];
            Console.WriteLine("Conectando ao dispositivo: " + userConfigs.Resource);

            try
            {
                tekVISA.Connect(userConfigs.Resource);
                Thread.Sleep(1000);
                Console.WriteLine("Conexão estabelecida com sucesso!");
                // CONFIGURAR CANAL E MEDIÇÕES
                
                tekVISA.SetMeasurement(measurements, userConfigs.Channel);

            }
            catch (Exception ex)
            {
                Console.WriteLine("Erro: " + ex.Message);
            }

            while (true)
            {
                Console.WriteLine(@"Configuração do ociloscópio
    1) Carregar a configuração existente
    2) Carregar uma nova configuração
    3) Carregar configuração Padrão");
                string oscilloscopeConfOp = Console.ReadLine();
                //string oscilloscopeConfOp = "3";
                if (!int.TryParse(oscilloscopeConfOp, out int oscilloscopeConfOpInt))
                {
                    if (!(oscilloscopeConfOpInt >= 1 && oscilloscopeConfOpInt <= 3))
                    {
                        Console.WriteLine($"Digite uma opção válida");
                    }

                }
                else
                {
                    switch (oscilloscopeConfOp)
                    {
                        case "1":
                            while (true)
                            {
                                Console.WriteLine("--> Por favor, insira o caminho completo do arquivo:");
                                oscilloscopeConfigsPath = Console.ReadLine().Replace("\\", "/");

                                // Verificar se o caminho existe
                                if (System.IO.File.Exists(oscilloscopeConfigsPath))
                                {
                                    Console.WriteLine("Caminho do arquivo fornecido: " + oscilloscopeConfigsPath + "\n\n");
                                    // Aqui você pode prosseguir com a leitura do arquivo ou outras operações
                                    storageServiceOscilloscopeConfigs.FilePath = oscilloscopeConfigsPath;
                                    oscilloscopeConfigs = storageServiceOscilloscopeConfigs.Load();
                                    tekVISA.Configure(oscilloscopeConfigs);

                                    storageServiceOscilloscopeConfigs.FilePath = userConfigsPath;
                                    UserConfigs configUser = storageServiceUserConfigs.Load();
                                    tekVISA.SetChannel(userConfigs.Channel);
                                    break;
                                }
                                else
                                {
                                    Console.WriteLine("O caminho fornecido não é válido ou o arquivo não existe.");
                                    continue;
                                }
                            }

                            break;
                        case "2":
                            // Implementar
                            Console.Write(@"!!ATENÇÂO!!
Faça as devidas configurações do trigger e de visualização (escala e posição vertical e horizontal)
no seu ociloscópio com base nos pulsos que deseja estudar. Depois disso pressione enter no teclado.");
                            Console.ReadLine();
                            tekVISA.Write("HEADER OFF");
                            OscilloscopeConfigs userOscConf = new OscilloscopeConfigs("", "", "", "");
                            userOscConf.DataFormatSet = "HEADER OFF;:DATA:SOU CH1;:DATA:ENCDG SRPbinary;:DATA:WIDTH 2;:DATA:START 1;:DATA:STOP";
                            userOscConf.AcquireSet = "ACQUIRE:STOPAFTER SEQUENCE;MODE SAMPLE;";

                            List<string> triggerConf = tekVISA.GetTriggerConf();
                            for (int k = 0; k < 5; k++)
                            { 
                                if (triggerConf.Count >= 6)
                                {
                                    userOscConf.TriggerSet =
                                        $"TRIG:MAI:MOD {triggerConf[0]};" +
                                        $"TYPE {triggerConf[1]};" +
                                        $"LEVEL {triggerConf[2]};" +
                                        $"VIDEO:SOURCE {triggerConf[3]};" +
                                        $":TRIG:MAI:EDGE:SLOPE {triggerConf[4]};" +
                                        $"COUP {triggerConf[5]};";
                                    Console.WriteLine("Quanto ao trigger...");
                                    for (int i = 0; i < 6; i++)
                                    {
                                        // Processar o dado, por exemplo:
                                        Console.WriteLine($"Dado {i}: {triggerConf[i]}");
                                    }
                                    break;
                                }
                                else
                                {
                                    Console.WriteLine("Configuração de trigger incompleta.");
                                }
                            }
                            List<string> visualizationConf = tekVISA.GetVisualizationConf();
                            if (visualizationConf.Count >= 4)
                            {
                                userOscConf.VisualizationSet =
                                    $"CH1:SCA {visualizationConf[0]};" +
                                    $"POS {visualizationConf[1]};" +
                                    $":HOR:SCA {visualizationConf[2]};" +
                                    $"POS {visualizationConf[3]};";
                                Console.WriteLine("Quanto ao VisualizationSet...");
                                for (int i = 0; i < 4; i++)
                                {
                                    // Processar o dado, por exemplo:
                                    Console.WriteLine($"Dado {i}: {visualizationConf[i]}");
                                }
                            }
                            else
                            {
                                Console.WriteLine("Configuração de visualização incompleta.");
                            }


                            storageServiceOscilloscopeConfigs.FilePath = directory + $"/{pathName}/userOscConf({pathName}).json";
                            storageServiceOscilloscopeConfigs.Save(userOscConf);
                            oscilloscopeConfigs = storageServiceOscilloscopeConfigs.Load();
                            tekVISA.Configure(oscilloscopeConfigs);
                            tekVISA.SetChannel(userConfigs.Channel);

                            break;
                        case "3":
                            // code block
                            storageServiceOscilloscopeConfigs.FilePath = $"{oscilloscopeConfigsPath}/oscilloscopeDefaultConfigs.json";
                            oscilloscopeConfigs = storageServiceOscilloscopeConfigs.Load();
                            tekVISA.Configure(oscilloscopeConfigs);
                            tekVISA.SetChannel(userConfigs.Channel);
                            break;

                    }
                    break;
                }
            }

            //tekVISA.SetMeasurementNone(userConfigs.Channel);
            string IMM_query = tekVISA.CompIMMQuery(measurements);
            // CAPTURAR DADOS DO PULSO
            while (countEvents < userConfigs.MaxEvents)
            {
                stopwatch.Start();
                tekVISA.Run();

                while (true) {
                    if (!tekVISA.WaitData()) { Console.WriteLine("no data...\n"); continue; }
                    break;
                }

                //data.Data = tekVISA.GetMeasurementsIMM(IMM_query);
                Thread.Sleep(1000);
                data.Data = tekVISA.GetData();

                //VERIFICA SE O ERRO 9.99E37 ESTÁ OCORRENDO
                //Console.WriteLine(data.Data[0]);
                if (tekVISA.esrTest()) { Console.WriteLine($"ESR: true");  continue; }

                stopwatch.Stop();
                Console.WriteLine($"Tempo passado: {stopwatch.Elapsed}");
                stopwatch.Reset();

                if (data.Data.Count() >= 4)
                {

                    /*data.Pkp = data.ConvertExpoStr(data.Data[0]);
                    data.RiseTime = data.ConvertExpoStr(data.Data[1]);
                    data.Nwidth = data.ConvertExpoStr(data.Data[2]);
                    data.FallTime = data.ConvertExpoStr(data.Data[3]);*/

                    //Console.WriteLine("Negative Width: {0}\nFall Time: {1}\nRise Time: {2}\nPeak-to-Peak: {3}", data.Nwidth, data.FallTime, data.RiseTime, data.Pkp);
                    // ADQUIRE TEMPO ATUAL

                    FileManager.update(data);
                    /*
                    DateTime tempoAtual = DateTime.Now;
                    horaAtual = tempoAtual.ToString("HH:mm:ss");
                    dataAtual = tempoAtual.ToString("dd/MM/yyyy");

                    Console.WriteLine("Data: " + dataAtual);
                    Console.WriteLine("Hora: " + horaAtual);
                    using (StreamWriter sw = File.AppendText(caminhoArquivo))
                    {
                        sw.Write((dataAtual + " | " + horaAtual).PadRight(10) + "\t");
                        // Iterando sobre cada elemento da lista 'data.Data'
                        foreach (string elemento in data.Data)
                        {
                            // Escrevendo o elemento formatado no arquivo
                            sw.Write(elemento.PadRight(10) + "\t"); // Usando + para concatenar
                        }
                        sw.WriteLine(); // Quebrar linha após cada linha da tabela

                    }
                    */
                    Console.WriteLine("Negative Width: {0}\nFall Time: {1}\nRise Time: {2}\nPeak-to-Peak: {3}", data.Data[0], data.Data[1], data.Data[2], data.Data[3]);
                    countEvents = countEvents + 1;
                    Console.WriteLine("Quantidade de Eventos: " + countEvents);
                }
            }

        }
    }
}