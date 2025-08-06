using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TekVISANet;
//using static System.Console;
using System.Diagnostics;
using System.Text.Json;

using comunicacaoOciloscopio.classes; //ACESSO À PASTA DE CLASSES

namespace TekOscilloscopeCommunication
{
    public class testes_diversos
    {

        

        public static void Main()
        {
            /*OscilloscopeConfigs hehe = new OscilloscopeConfigs("", "", "", "");

            hehe.DataFormatSet = "HEADER OFF;:DATA:SOU CH1;:DATA:ENCDG SRPbinary;:DATA:WIDTH 2;:DATA:START 1;:DATA:STOP";
            hehe.AcquireSet = "ACQUIRE:STOPAFTER SEQUENCE;MODE SAMPLE;";
            hehe.TriggerSet = "TRIG:MAI:MOD NORMAL;TYPE EDGE;LEVEL -8.0E-1;VIDEO:SOURCE CH1;:TRIG:MAI:EDGE:SLOPE FALL;COUP DC;";
            hehe.VisualizationSet = "CH1:SCA 2;POS 1;:HOR:SCA 5.0E-9;POS 0.0E0";

            var storageServiceOscilloscopeConfigs = new JsonStorageService<OscilloscopeConfigs>(
                "C:/Users/projetoMCA/Desktop/kauaWorkspace/projetoComunicacao/comunicacaoOciloscopio_latest_samuca_changes/oscilloscopeDefaultConfigs.json"
            );
            storageServiceOscilloscopeConfigs.Save(hehe);*/

            /*
            // Inicializar OpenFileDialog
            TekVISA tekVISA = new TekVISA();
            UserConfigs userConfigs = new UserConfigs(" ", " ", " ", 0, " ");

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
                Console.WriteLine("Conexão estabelecida com sucesso!");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Erro: " + ex.Message);

            }
            */
            //Console.WriteLine(tekVISA.SetMeasurement(new List<string> { "NWIDTH", "FALL", "RISE", "PK2PK" }, userConfigs.Channel));

            /*Console.WriteLine("Testando trigger...");
            List<string> triggerConf = tekVISA.GetTriggerConf();
            for (int i = 0; i < 6; i++)
            {
                // Processar o dado, por exemplo:
                Console.WriteLine($"Dado {i}: {triggerConf[i]}");
            }*/



            /*DateTime currentTime = DateTime.UtcNow;
            double julianDate = Tools.ConvertDateToJulian(currentTime);
            Console.WriteLine(Math.Round(julianDate,6));*/

            /*
            // Obtém a data e hora atual
            DateTime currentDate = new DateTime(2000, 1, 1, 0, 0, 0);

            // Converte para data juliana com frações de dias
            double julianDate = Tools.ConvertDateToJulian(currentDate, 6);

            DateTime result = Tools.ConvertJulianToGregorian(julianDate, 6);

            DateTime convertedDate = Tools.ConvertUtcToTimeZone(result, "E. South America Standard Time");

            Console.WriteLine($"Data Juliana: {julianDate}");
            Console.WriteLine($"Data Gregoriana (UTC): {result:yyyy-MM-dd HH:mm:ss}");
            Console.WriteLine($"Data Gregoriana (GMT -3): {convertedDate:yyyy-MM-dd HH:mm:ss}");
            */
            Console.WriteLine("Message sent to the broadcast address");
            string response = "";

            Console.WriteLine("Hello World!");
            VISA TVA = new VISA();
            ArrayList resources = new ArrayList();
            TVA.FindResources("?*", out resources); //Searching for oscilloscope connected on PC

            //Showing the osciloscope ID
            foreach (string s in resources)
            {
                Console.WriteLine(s);
            }

            TVA.Open(resources[0].ToString()); //Connecting with the oscilloscope
            TVA.Write("*IDN?");
            bool status = TVA.Read(out response);
            if (status)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("IDN (device information):"); //ID oscilloscope
                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine(response);
            }



            TVA.Write("HEADER OFF");
            //Setting the oscilloscope for save data
            TVA.Write("DATA:SOU CH1");

            //Setting the wave length that will be stored
            TVA.Write("DATA:ENCDG SRPbinary");
            TVA.Write("DATA:WIDTH 2");
            TVA.Write("DATA:START 1");
            TVA.Write("DATA:STOP 2500");
            List<string> measurements = new List<string> { "NWIDTH", "FALL", "RISE", "PK2PK" };
            string query_str = "";
            string meas;
            List<string> data = new List<string> { };
            List<string> all = new List<string> { };
            for (int i = 0; i < measurements.Count(); i++)
            {
                if (i > 0)
                {
                    query_str += ";:";

                }
                query_str += $"MEASU:IMM:TYPE {measurements[i]};:";
                query_str += $"MEASU:IMM:VAL?";
            }
            all = query_str.Split(';').ToList();

            void SetMeasurementNone(string channel)
            {
                for (int i = 0; i < 5; i++)
                {
                    TVA.Write($"MEASUREMENT:MEAS{i + 1}:SOURCE {channel}");
                    TVA.Write($"MEASUREMENT:MEAS{i + 1}:TYPE NONE");
                    TVA.Query("*ESR?", out string outh);
                    Console.WriteLine($"MEASUREMENT:MEAS{i + 1}:SOURCE {channel};:MEASUREMENT:MEAS{i + 1}:TYPE NONE");
                }
            }

            SetMeasurementNone("CH1");

            /*string IMM_query = tekVISA.CompIMMQuery(measurements);
            List<string> res = tekVISA.GetMeasurementsIMM(IMM_query);

            Console.WriteLine(all[0]);
            Console.WriteLine(query_str);
            for (int i = 0; i < res.Count(); i++)
            {
                Console.WriteLine($"Medida índice {i}: {res[i]}");
            }*/
        }
    }

    /*
    This code produces the following output.

    April 3, 2002 of the Gregorian calendar equals the following in the Julian calendar:
       Era:        1
       Year:       2002
       Month:      3
       DayOfYear:  80
       DayOfMonth: 21
       DayOfWeek:  Wednesday

    After adding two years and ten months:
       Era:        1
       Year:       2005
       Month:      1
       DayOfYear:  21
       DayOfMonth: 21
       DayOfWeek:  Thursday

    */
}
