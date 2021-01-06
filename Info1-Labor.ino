// Autor: heph1030
// Datum: 04.01.2021
// Zweck: Zeitraffervorrichtung:
//        - 2 Servos
//        - 1 OLED-Display
//        - 3 Taster


// Bibliotheken:
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Anschlüsse:
int pinTaster1 = 14;
int pinTaster2 = 12;
int pinTaster3 = 13;
int servo1Pin = 32;
int servo2Pin = 23;

// Servo:
Servo servo1;
Servo servo2;
int pos;                                                                    // Position zwischen 0° und 180°
bool up = true;                                                             // Richtung der Servos
bool start;                                                                 // lässt Servos "loslaufen"

// Timer:
unsigned long programmlaufzeit = 0;                                         // Zeit seit Programmstart [ms]
unsigned long servolaufzeit = 0;                                            // Zeit seit Servostart [ms]                                  
unsigned long previousDisplay = 0;                                          // Zeit der letzten Ausführung [ms]
unsigned long previousButtons = 0;
unsigned long previousServos = 0 ;                                          
unsigned long previousReset = 0;
unsigned long intervallMillis = 60000;                                      // Intevervall[ms]
unsigned long schrittMillis;
int intervallButtons = 1000;                                                // buttons() wird jede Sekunde abgefragt [ms]
int intervallDisplay = 1000;                                                // Display wird jede Sekunde aktualisiert [ms]
unsigned long servoStart = 0;                                               // Servostartzeit [ms]


// Display:
#define OLED_RESET    4
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
 Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  
  // Konsole:
  Serial.begin(115200);


  // Eingabe:
  pinMode(pinTaster1, INPUT_PULLUP);
  pinMode(pinTaster2, INPUT_PULLUP);
  pinMode(pinTaster3, INPUT_PULLUP);


  // Servos:
  servo1.attach(servo1Pin);                                                // Anschluss
  servo2.attach(servo2Pin);
  start = false;                                                           // Servos werden pausiert
 

  // Display:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(INVERSE);
  display.setCursor(4,4);
  display.print("Hello :)");
  display.display();
  delay(4000);
  display.clearDisplay();
  display.setTextSize(1);
}

void loop() 
{
  
    programmlaufzeit = millis();
  
    buttons();
   
    servoSteuerung();
     if(start == true)
  {
    servolaufzeit = millis() - servoStart;
  }
    displayAnzeige();
   
}

void buttons()
{
  if(programmlaufzeit - previousButtons >= intervallButtons)
  {
      previousButtons = programmlaufzeit;
     
      if(digitalRead(pinTaster1) == LOW)                                    // Zeit erhöhen
      {
        
        intervallMillis += schrittweite();
      }
      else if(digitalRead(pinTaster2) == LOW)                               // Standartwert
      {
        intervallMillis = 60000; // kleinster Wert = 1min
        
      }
      else if(digitalRead(pinTaster3) == LOW)                               // Start / Neustart
      {
        if(start == false)
        {
          start = true;
          previousReset = 0;
          
         while(servo1.read() > 0)
         {
          pos = servo1.read();
          programmlaufzeit = millis();
          if(programmlaufzeit - previousReset >= 50)
          {
            previousReset = programmlaufzeit;
            pos--;
            servo1.write(pos);
          }
         }
         
         while(servo2.read() > 0)
         {
          pos = servo2.read();
          programmlaufzeit = millis();
          if(programmlaufzeit - previousReset >= 50)
          {
            previousReset = programmlaufzeit;
            pos--;
            servo2.write(pos);
          }
         }
          
         servoStart = programmlaufzeit;
          
        }
        else
        {
          start = false;
        }
        
     }
  }
}

unsigned long schrittweite()
{
  if(intervallMillis == 60000)                                              // 1 min
  {
    schrittMillis = 60000;                                                  // 1 min
  }
  else if(intervallMillis < 600000)                                         // kleiner 10 min 
  {
    schrittMillis = 120000;                                                 // 2 min
  }
  else if(intervallMillis < 1200000)                                        // kleiner 20 min
  {
    schrittMillis = 300000;                                                 // 5 min
  }
  else if(intervallMillis < 1800000)                                        // kleiner 30 min
  {
    schrittMillis = 600000;                                                 // 10 min
  }
  else if(intervallMillis < 3600000)                                        // kleiner 1h
  {
    schrittMillis = 900000;                                                 // 15 min
  }
  else if(intervallMillis < 7200000)                                        // kleiner 2h
  {
    schrittMillis = 1800000;                                                // 30 min
  }
  else if(intervallMillis < 14400000)                                       // kleiner 4h
  {
    schrittMillis = 7200000;                                                // 2h
  }
  else if(intervallMillis < 43200000)                                       // kleiner 12h
  {
    schrittMillis = 14400000;                                               // 4h
  }
  else                                                                      // ab 12h
  {
    schrittMillis = 43200000;                                               // 12h
  }
 return schrittMillis;
}


void servoSteuerung()
{
  if (programmlaufzeit - previousServos >= (intervallMillis/180))           // verzögerung 
    {
      if(start == true)
      {
        previousServos = programmlaufzeit;                                  // Zeit des letzten Intervallstarts speichern
        if((pos > 0)&&(up == true)&&(pos < 180))
        {
          pos++;
          servo1.write(pos);
          servo2.write(pos);
          
        }
        else if((pos < 180)&&(up == false)&&(pos > 0))
        {
          pos--;
          servo1.write(pos);
          servo2.write(pos);
          
        }
        else if(pos == 0)
        {
          pos++;
          servo1.write(pos);
          servo2.write(pos);
          up = true;
          
        }
        else                  //pos == 180
        {
          pos--;
          servo1.write(pos);
          servo2.write(pos);
          up = false;
          
         }
      }
    }
}

void displayAnzeige()
{
  display.clearDisplay();
  display.setCursor(20,20);
   if (programmlaufzeit - previousDisplay >= intervallDisplay)      
    {
      previousDisplay = programmlaufzeit;
     
      
      // aktuelle servolaufzeit:
      if(berechneMinuten(servolaufzeit) >= 60)
      {
        display.print(berechneStunden(servolaufzeit));
        display.print(" Stunden");
      }
      else if(berechneSekunden(servolaufzeit) >= 60)
      {
        display.print(berechneMinuten(servolaufzeit));
        display.print(" Minuten");
      }
      else
      {
        display.print(berechneSekunden(servolaufzeit),0);
        display.print(" Sekunde/n");
      }


      //Linie:
      display.drawLine(10,33,110,33,INVERSE);

      
      //Zeit für halbe Periode:
      display.setCursor(20,40);
      
      if(berechneMinuten(intervallMillis) >= 60)
      {
        display.print(berechneStunden(intervallMillis));
        display.print(" Stunde/n");
      }
      else if(berechneSekunden(intervallMillis) >= 60)
      {
        display.print(berechneMinuten(intervallMillis));
        display.print(" Minute/n");
      }
      else
      {
        display.print(berechneSekunden(intervallMillis));
        display.print(" Sekunde/n");
      }
      display.display();
    }
}

float berechneSekunden(unsigned long x)
{
  return (x/1000.0);
}

float berechneMinuten(unsigned long x)
{
  return (x/60000.0);
}

float berechneStunden(unsigned long x)
{
  return (x/3600000.0);
}
