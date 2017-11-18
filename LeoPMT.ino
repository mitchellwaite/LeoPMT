#include <Key.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include "Keyboard.h"
#include <SPI.h>
#include <SD.h>
#include <AES.h>
#include <Crypto.h>
#include <SHA256.h>
#include <RNG.h>

SoftwareSerial mySerial(A1, A0); // RX, TX


const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns

char numberKeys[ROWS][COLS] = {
    { '1','2','3', 'A' },
    { '4','5','6', 'B' },
    { '7','8','9', 'C' },
    { ' ','0','#', 'D' }
};

byte colPins[COLS] = {A2, A3, A4, A5}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {3, 4, 5, 6}; //connect to the column pinouts of the keypad
Keypad numpad( makeKeymap(numberKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );

File root;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 10;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on Arduino Uno boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(SS, OUTPUT);
  SD.begin(10,11,12,13);

  Keyboard.begin();

  // Set up the LCD screen
  mySerial.begin(9600);
  clearLCD();

  if (SD.exists("mkey.dat")){
    //get a passcode
  } else{
    //Set up a new encryption key
  }
  
}

void loop() { // run over and over
  char keyBuf[32] = {'\0'};
  char shaBuf[32] = {'\0'};
  
  getPasscode(keyBuf, 32, "Enter Passcode:",false);

  clearLCD();
  mySerial.println("Passcode was:");
  mySerial.print(keyBuf);

  calcSHA(keyBuf, 32, shaBuf, 32);

  Serial.print("sha: ");
  
  for(int i = 0;i<32;i++)
  {
    Serial.print(shaBuf[i]);
  }
  
  delay(2000);
}

void getPasscode(char * keyBuffer, size_t bufSz, String message, bool showChars)
{
  int keyCount = 0;
  
  clearLCD();

  mySerial.println(message);

  while(true)
  {
    char key = numpad.getKey();
   
    if (key){

      if (key == '*' || key == '#'){
        break;
      }
      else if(key >= 48 && key <= 57)
      {
        if(keyCount < 15)
        {
          if(showChars)
          {
            mySerial.print(key);
          }
          else
          {
            mySerial.print('*');
          }
        }

        if(keyCount < bufSz)
        {
          keyBuffer[keyCount++] = key;
        }
      }
    }
  }

  keyBuffer[bufSz] = '\0';
   
  return;
}

void clearLCD()
{
  mySerial.write(0xFE);
  mySerial.write(0x58);
  mySerial.write(0xFE);
  mySerial.write(0x48);
}

void calcSHA(void* inBuf, int bufSz, void* outBuf, int outBufSz)
{
  SHA256 sha256;
  sha256.reset();
  sha256.update(inBuf, bufSz);
  sha256.finalize(outBuf, outBufSz);
}

void getRNG()
{
  
}

