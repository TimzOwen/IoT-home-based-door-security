#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#include <Key.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define Password_Lenght 7 // Give enough room for six chars + NULL char

LiquidCrystal_I2C lcd(0x27,16,2);
char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "123456"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','4','7', '*'},
  {'2','5','8', '0'},
  {'3','6','9', '#'},
  {'A','B','C', 'D'}
};

byte rowPins[ROWS] = {
  5,4,3,2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  9,8,7,6}; //connect to the column pinouts of the keypad

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad 

#define ACCEPTED_PIN 10
#define DENIED_PIN 11
#define DOOR_PIN 50

Servo servo;

#define FONA_RX 44
#define FONA_TX 45
#define FONA_RST 46

char sendto[] = "+254724522321";
char message[] = "ALERT!!! Someone tried to access your house!";
   

char replybuffer[255];

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void setup()
{
  lcd.init();// initialize the lcd 
  lcd.backlight();

  Serial.begin(115200);
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));
  fonaSS.begin(4800); 
  if (! fona.begin(fonaSS)) {            
    Serial.println(F("Couldn't find FONA"));
  }
  Serial.println(F("FONA is OK"));

  servo.attach(DOOR_PIN);

  pinMode(ACCEPTED_PIN, OUTPUT);
  pinMode(DENIED_PIN, OUTPUT);
}

void loop()
{
  servo.write(0);
  lcd.setCursor(0,0);
  lcd.print("Enter Password");

  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count,1); // move cursor to show each new char
    lcd.print(Data[data_count]); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
  {
   
    if(!strcmp(Data, Master)){ // equal to (strcmp(Data, Master) == 0)
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ACCESS  ");
      lcd.setCursor(0 ,1);
      lcd.print("GRANTED");
  
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WELCOME ");

      digitalWrite(ACCEPTED_PIN, HIGH);
       digitalWrite(DENIED_PIN, LOW);
       delay(3000);

       servo.write(90);
       delay(5000);
       
    }
    else{
      lcd.print("Bad");
      digitalWrite(DENIED_PIN, HIGH);
      delay(300);
      digitalWrite(DENIED_PIN, LOW);
       delay(300);
       digitalWrite(DENIED_PIN, HIGH);
       delay(300);
       digitalWrite(DENIED_PIN, LOW);
       delay(300);
       digitalWrite(DENIED_PIN, HIGH);
       digitalWrite(ACCEPTED_PIN, LOW);

       Serial.print(F("Sending alert to "));
       Serial.println(sendto);
      
       if (!fona.sendSMS(sendto, message)) {
          Serial.println(F("Failed"));
       } else {
          Serial.println(F("Sent!"));
       }
       
    }
    delay(1000);// added 1 second delay to make sure the password is completely shown on screen before it gets cleared.
    lcd.clear();
    clearData();   
  }
}

void openDoor(){
    
  }

void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}
