using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Threading;

namespace SHETALPO_Time
{
    class Program
    {
        static double currentTemp;

        static double targetTemp;

        static int heatValue;


        //Termy roznica temperatur
        static double[] DiffVeryLow;
        static double[] DiffLow;
        static double[] DiffBalanced;
        static double[] DiffMedium;
        static double[] DiffHigh;
        static double[] DiffVeryHigh;



        //Termy Stopien grzania

        static double[] HeatZero;
        static double[] HeatLow;
        static double[] HeatVLow;
        static double[] HeatMedium;
        static double[] HeatHigh;
        static double[] HeatVeryHigh;


        static double[] HeatZeroCopy;
        static double[] HeatLowCopy;
        static double[] HeatVLowCopy;
        static double[] HeatMediumCopy;
        static double[] HeatHighCopy;
        static double[] HeatVeryHighCopy;

        private static SerialPort port = new SerialPort("COM6", 9600, Parity.None, 8, StopBits.One);

        static string hour = "";
        static string minute = "";
        static void Main(string[] args)
        {
            //Inicjalizacja termów;

            DiffVeryLow  = new double[256];
            DiffLow = new double[256];
            DiffBalanced = new double[256];
            DiffMedium = new double[256];
            DiffHigh = new double[256];
            DiffVeryHigh = new double[256];

            HeatZero = new double[256];
            HeatLow = new double[256];
            HeatMedium = new double[256];
            HeatVLow = new double[256];
            HeatHigh = new double[256];
            HeatVeryHigh = new double[256];

            HeatZeroCopy = new double[256];
            HeatLowCopy = new double[256];
            HeatMediumCopy = new double[256];
            HeatVLowCopy = new double[256];
            HeatHighCopy = new double[256];
            HeatVeryHighCopy = new double[256];


            generateTerms();


            port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
            port.DtrEnable = true;


            port.Open();

            while (true)
            {
                

                string newhour = DateTime.Now.Hour.ToString();
                string newminute = DateTime.Now.Minute.ToString();

                //if(hour != newhour || minute != newminute)
                //{
                //    port.WriteLine(DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString());
                //    //Thread.Sleep(2000);

                //    hour = newhour;
                //    minute = newminute;
                //}


                try
                {
                    Console.WriteLine(targetTemp);
                    heatValue = simulate(currentTemp, targetTemp);
                    Console.WriteLine("Wartość grzania " +heatValue);

                    port.WriteLine(DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + heatValue);
                }
                catch (Exception e)
                {
                  
                }

                Thread.Sleep(3000);
                
            }
        }

        private static void generateTerms()
        {
            

            for(int i = 0; i <= 255; i++)
            {
                double temp = i;
                //Mała
                if (i >= 0 && i <= 30)
                {
                    DiffLow[i] = ((temp - 0.0) /(30.0 - 0.0));
                }
                else if(i >=30 && i <= 50)
                {
                    DiffLow[i] = ((50.0 - temp) / (50.0 - 30.0));
                }
                else
                {
                    DiffLow[i] = 0.0;
                }
                

                //Umiarkowana
                if (i >= 40 && i <= 70)
                {
                    DiffBalanced[i] = ((temp - 40.0) / (70.0 - 40.0));
                }
                else if (i >= 70 && i <= 100)
                {
                    DiffBalanced[i] = ((100.0 - temp) / (100.0 - 70.0));
                }
                else
                    DiffBalanced[i] = 0.0;

                //Średnia
                if(i>=90 && i <= 120)
                {
                    DiffMedium[i] = ((temp - 90.0) / (120.0 - 90.0));
                }
                else if(i >=120 && i <= 150)
                {
                    DiffMedium[i] = ((150.0 - temp) / (150.0 - 120.0));
                }
                else
                {
                    DiffMedium[i] = 0.0;
                }


                //Wysoka

                if (i >= 140 && i <= 170)
                {
                    DiffHigh[i] = ((temp - 140.0) / (170.0 - 140.0));

                }
                else if (i >= 170 && i <= 200)
                {
                    DiffHigh[i] = ((200.0 - temp) / (200.0 - 170.0));
                }    
                else
                {
                    DiffHigh[i] = 0.0;
                }
                    


                //Bardzo wysoka

                if (i >= 190 && i <= 250)
                {
                    DiffVeryHigh[i] = ((temp - 190.0) / (250.0 - 190.0));
                }
                else if(i >250)
                {
                    DiffVeryHigh[i] = 1.0;
                }
                else
                {
                    DiffVeryHigh[i] = 0.0;
                }


                
            }


            //Stopien grzania

            for (int i = 0; i < 256; ++i)
            {
                double temp = i;
                //Zero
                if (i >= 0 && i<= 44)
                {
                    HeatZero[i] = (temp - 0.0)/(44.00-0);
                }
                else if(i >= 44 && i <= 89)
                {
                    HeatZero[i] = (89.0 - temp)/(89.0 - 44.0);
                }

                //Małe
                if (i >= 51 && i <= 89)
                {
                    HeatLow[i] = (temp - 51) / (89.0 - 51.0);
                }
                else if (i >= 89 && i <= 127)
                {
                    HeatLow[i] = (127.0 - temp) / (127.0 - 89.0);
                }
                else
                {
                    HeatLow[i] = 0.0;
                }



                //Srednie grzanie
                if (i >= 89 && i <= 127)
                {
                    HeatMedium[i] = (temp - 89.0) / (127.0 - 89.0);
                }
                else if (i >= 127 && i <= 166)
                {
                    HeatMedium[i] = (166.0 - temp) / (166.0 - 127.0);
                }
                else
                {
                    HeatMedium[i] = 0.0;
                }


                //Duze grzanie
                if (i >= 127 && i <= 166)
                {
                    HeatHigh[i] = (temp - 127.0) / (166.0 - 127.0);
                }
                else if (i >= 166 && i <= 204)
                {
                    HeatHigh[i] = (204.0 - temp) / (204.0 - 166.0);
                }
                else
                {
                    HeatHigh[i] = 0.0;
                }

                //Bardzo duze grzanie
                if (i >= 166 && i <= 210)
                {
                    HeatVeryHigh[i] = (temp - 166.0) / (210.0 - 166.0);
                }
                else if (i >= 210)
                {
                    HeatVeryHigh[i] = 1.0;
                }
                else
                {
                    HeatVeryHigh[i] = 0.0;
                }

            }
            }
       
        private static int simulate(double currentTemp, double targetTemp)
        {
            
            int tempDiff = (int)((targetTemp - currentTemp));

            if (tempDiff < 0)
            {
                heatValue = 0;
                return heatValue;
            }

            if(tempDiff > 255)
            {
                heatValue = 255;
                return heatValue;
            }

            //if (tempDiff != 0)
            //

                double diffLow = DiffLow[tempDiff];
                double diffBalanced = DiffBalanced[tempDiff];
                double diffMedium = DiffMedium[tempDiff];
                double diffHigh = DiffHigh[tempDiff];
                double diffVeryHigh = DiffVeryHigh[tempDiff];

                double heatVarLack = diffLow;
                double heatVarLow = diffBalanced;
                double heatVarMed = diffMedium;
                double heatVarHigh = diffHigh;
                double heatVarVeryHigh = diffVeryHigh;


                for (int i = 0; i < 256; ++i)
                {
                    HeatZeroCopy[i] = Math.Min(heatVarLack, HeatZero[i]);
                    HeatLowCopy[i] = Math.Min(heatVarLow, HeatLow[i]);
                    HeatMediumCopy[i] = Math.Min(heatVarMed, HeatMedium[i]);
                    HeatHighCopy[i] = Math.Min(heatVarHigh, HeatHigh[i]);
                    HeatVeryHighCopy[i] = Math.Min(heatVarVeryHigh, HeatHigh[i]);
            }


                double[] aggregated = new double[256];

                for (int i = 0; i < 256; ++i)
                { 
                    aggregated[i] = Math.Max(Math.Max(Math.Max(HeatZeroCopy[i], HeatLowCopy[i]), Math.Max(HeatMediumCopy[i], HeatHighCopy[i])), Math.Max(HeatHighCopy[i], HeatVeryHighCopy[i]));
                }


                double sumLicznik = 0;
                double sumMianownik = 0;

                for (int i = 0; i < 256; ++i)
                {
                    sumLicznik += aggregated[i] *i;
                    sumMianownik += aggregated[i];


                }
                Console.WriteLine((sumLicznik / sumMianownik));
                heatValue = (int)((sumLicznik / sumMianownik));



            
            return heatValue;
        }

        
        private static void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            port.DiscardOutBuffer();
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadLine();
            Console.WriteLine(indata);
            //Show all the incoming data in the port's buffer
            //Console.WriteLine(indata);   
                var data = indata.Split(' ');

            currentTemp = Convert.ToDouble(data[0]);

            targetTemp = Convert.ToDouble(data[1]);



            port.DiscardInBuffer();

        }
    }
}
