#include <DHT.h>
#include <LiquidCrystal.h>

//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTPIN 2     // what pin we're connected to


//Buttons declaration
#define BUTT_UP A3
#define BUTT_DOWN A2
#define BUTT_OK A1
#define BUTT_MENU A0
#define DHT_PIN 2

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
DHT dht(DHTPIN, DHTTYPE);



bool wasClicked = false;

int ledPin = 3;

int a = 10;
int currentView = 0;

float TEMPERATURE;
float targetTemperature;

int HOUR = 12;
int targetHOUR = 12;
int MINUTE = 0;
int targetMINUTE = 0;
int heatingDegree;




int menuMode = 0;
int configMode = 0;

int timeMode = 0;

int incomingByte = 0; // for incoming serial data

float tempDiff;



char cstr[16];






//Funkcje

void ftoa(float f, char *str, uint8_t precision) {
  uint8_t i, j, divisor = 1;
  int8_t log_f;
  int32_t int_digits = (int)f;             //store the integer digits
  float decimals;
  char s1[12];

  memset(str, 0, sizeof(s1));  
  memset(s1, 0, 10);

  if (f < 0) {                             //if a negative number 
    str[0] = '-';                          //start the char array with '-'
    f = abs(f);                            //store its positive absolute value
  }
  log_f = ceil(log10(f));                  //get number of digits before the decimal
  if (log_f > 0) {                         //log value > 0 indicates a number > 1
    if (log_f == precision) {              //if number of digits = significant figures
      f += 0.5;                            //add 0.5 to round up decimals >= 0.5
      itoa(f, s1, 10);                     //itoa converts the number to a char array
      strcat(str, s1);                     //add to the number string
    }
    else if ((log_f - precision) > 0) {    //if more integer digits than significant digits
      i = log_f - precision;               //count digits to discard
      divisor = 10;
      for (j = 0; j < i; j++) divisor *= 10;    //divisor isolates our desired integer digits 
      f /= divisor;                             //divide
      f += 0.5;                            //round when converting to int
      int_digits = (int)f;
      int_digits *= divisor;               //and multiply back to the adjusted value
      itoa(int_digits, s1, 10);
      strcat(str, s1);
    }
    else {                                 //if more precision specified than integer digits,
      itoa(int_digits, s1, 10);            //convert
      strcat(str, s1);                     //and append
    }
  }

  else {                                   //decimal fractions between 0 and 1: leading 0
    s1[0] = '0';
    strcat(str, s1);
  }

  if (log_f < precision) {                 //if precision exceeds number of integer digits,
    decimals = f - (int)f;                 //get decimal value as float
    strcat(str, ".");                      //append decimal point to char array

    i = precision - log_f;                 //number of decimals to read
    for (j = 0; j < i; j++) {              //for each,
      decimals *= 10;                      //multiply decimals by 10
      if (j == (i-1)) decimals += 0.5;     //and if it's the last, add 0.5 to round it
      itoa((int)decimals, s1, 10);         //convert as integer to character array
      strcat(str, s1);                     //append to string
      decimals -= (int)decimals;           //and remove, moving to the next
    }
  }
}



void initLCD(){
    currentView=1;
    lcd.setCursor(0, 0); //Ustawienie kursora
    lcd.print("SHETALPO");
    delay(3000);
    lcd.clear();
    lcd.print("LOADING");
    for(int i = 0; i < 15; i++)
    {
      lcd.setCursor(i, 1);
      lcd.write("*");
      delay(300); 
    }

    
    showMenu();
    
}

void showMenu(){
    currentView = 3;
    menuMode = 0;
    lcd.clear();
    lcd.println("1. STATUS    <-   ");
    lcd.setCursor(0,1);
    lcd.println("2. KONFIGURUJ     ");
}

void showStatus(){
    currentView = 2;
    lcd.clear();
    lcd.print("TEMP: ");
    ftoa(TEMPERATURE, cstr, 1);
    lcd.print(TEMPERATURE);
    
    lcd.setCursor(0,1);
    //lcd.println("TIME: 12:00      ");
    lcd.print("TIME: ");
    if(HOUR < 10){
        lcd.print("0");
    }
    lcd.print(HOUR);
    lcd.print(":");
    if(MINUTE < 10){
        lcd.print("0");
    }
    lcd.print(MINUTE);
}

void showConfig(){
    currentView = 4;
    lcd.clear();
    lcd.print("TEMP: ");
    ftoa(targetTemperature, cstr, 1);
    lcd.print(targetTemperature);
    lcd.print("*C   <-");
    lcd.setCursor(0,1);
    lcd.print("TIME: ");

    if(targetHOUR < 10){
        lcd.print("0");
        lcd.print(itoa(targetHOUR, cstr, 10));
    }else{
        lcd.print(itoa(targetHOUR, cstr, 10));
    }
    
    lcd.print(":");

    if(targetMINUTE < 10){
        lcd.print("0");
        lcd.print(targetMINUTE);
    }else{
        lcd.print(targetMINUTE);
    }
}

void showMenuDown(){
    currentView = 3;
    menuMode = 1;
    lcd.clear();
    lcd.println("1. STATUS         ");
    lcd.setCursor(0,1);
    lcd.println("2. KONFIGURUJ<-   ");

}

void showSettingTime(){
    currentView = 4;
    lcd.clear();
    lcd.print("TEMP: ");
    ftoa(targetTemperature, cstr, 1);
    lcd.print(cstr);
    lcd.print("*C");
    lcd.setCursor(0,1);
    // lcd.println("TIME: 12:00  <-   ");
    lcd.print("TIME: ");

    if(targetHOUR < 10){
        lcd.print("0");
        lcd.print(itoa(targetHOUR, cstr, 10));
    }else{
        lcd.print(itoa(targetHOUR, cstr, 10));
    }
    
    lcd.print(":");

    if(targetMINUTE < 10){
        lcd.print("0");
        lcd.print(targetMINUTE);
    }else{
        lcd.print(targetMINUTE);
    }

    if(timeMode == 0){
        lcd.print(" [H]  <-");
    }else if(timeMode == 1){
        lcd.print(" [M]  <-");
    }
    

  
}



String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
} 


void setup()
{
     Serial.begin(9600);
     lcd.begin(16, 2); //Deklaracja typu
     delay(5000);
     initLCD();
     dht.begin();

     if(isnan(dht.readTemperature()))
     {
       lcd.println("Błąd inicjalizacji czujnika !");
       lcd.setCursor(0,1);
       lcd.println("Sprawdz czujnik!");
       delay(60000);
     }

     TEMPERATURE = dht.readTemperature();
     targetTemperature = TEMPERATURE;
     
     pinMode(BUTT_MENU, INPUT_PULLUP);
     pinMode(BUTT_DOWN, INPUT_PULLUP);
     pinMode(BUTT_UP, INPUT_PULLUP);
     pinMode(BUTT_OK, INPUT_PULLUP);
 
}

void loop()
{
    
    delay(3000);
    
    TEMPERATURE = dht.readTemperature();
    int t1 = (int)(TEMPERATURE*10);
    Serial.print(t1);
    Serial.print(" ");
    int t2 = (int)(targetTemperature*10);
    Serial.println(t2);
        
    

    if (Serial.available() > 0) {
        
        String in = Serial.readString();
        heatingDegree = getValue(in, ':', 2).toInt();
        analogWrite(ledPin, heatingDegree);
        
        if(currentView == 2)
        {
         
        HOUR = getValue(in, ':', 0).toInt();
        MINUTE = getValue(in, ':', 1).toInt();
        showStatus();
        }
    }

    if(digitalRead(BUTT_OK)==LOW){
            
            wasClicked = true;
        }else{
            
            wasClicked = false;
        }
    //showStatus();


     if(digitalRead(BUTT_MENU)==LOW){
        
         showMenu();
     }

     //Check buttons in menu view
     if(currentView==3){
        if(digitalRead(BUTT_UP)==LOW){
            
          menuMode = 0;
          showMenu();
          
        }else if(digitalRead(BUTT_DOWN)==LOW){
          menuMode=1;
          
          showMenuDown();
        }

     
         if(digitalRead(BUTT_OK)==LOW){
              
             if(menuMode==0){
                 
                 //Serial.println(menuMode);
                 
                 showStatus();
             }
             else if(menuMode==1){
                
                 showConfig();
                 return;
             }
         }
     }
    
     //Check buttons in status view
     if(currentView==2){
         //Menu only

     }

    if(currentView==4){
        if(configMode==0){

            if(digitalRead(BUTT_UP)==LOW){
                targetTemperature+=0.1;
                
               
                showConfig();
                


            }

            else if(digitalRead(BUTT_DOWN)==LOW){
                targetTemperature-=0.1;
                
                showConfig();
            }

        TEMPERATURE = dht.readTemperature();
        int t1 = (int) (TEMPERATURE*10);
        Serial.print(t1);
        Serial.print(" ");
        int t2 = (int)(targetTemperature*10);
        Serial.println(t2);
        

        

        if(digitalRead(BUTT_OK)==LOW && !!wasClicked){
          
          configMode=1;
          showSettingTime();
        }
      }else if(configMode==1){
          if(digitalRead(BUTT_UP)==LOW){

              if(timeMode == 0){
                
                  targetHOUR = targetHOUR + 1;
                    if(targetHOUR >= 24){
                        
                        targetHOUR = 0;
                    }

              }else if(timeMode == 1){
                 
                  targetMINUTE = targetMINUTE + 1;

                  if(targetMINUTE >= 60){
                    
                      targetMINUTE = 0;
                      targetHOUR = targetHOUR + 1;
                  }
              }
                showSettingTime();
            }

             if(digitalRead(BUTT_DOWN)==LOW){
                if(timeMode == 0)
                {
                    
                  targetHOUR = targetHOUR - 1;
                    if(targetHOUR <= -1){
                        targetHOUR = 23;
                    }

              }else if(timeMode == 1){
                  
                  targetMINUTE = targetMINUTE - 1;
                  if(targetMINUTE <= -1){
                    
                      targetMINUTE = 59;
                      targetHOUR = targetHOUR - 1;
                  }
              }
                showSettingTime();
            }

         if(digitalRead(BUTT_OK)==LOW && !wasClicked && timeMode == 0){
             //ASSIGN temp if config state == 0
             
             timeMode = 1;
             
             showSettingTime();

         }else if(digitalRead(BUTT_OK)==LOW && !wasClicked && timeMode == 1){
            
             timeMode = 0;
             configMode = 0;
             showMenu();
         }
     }
             

             //ASSIGN time if config state == 1


         }

        

     }
