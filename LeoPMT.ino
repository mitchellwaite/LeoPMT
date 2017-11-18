#include <Key.h>
#include <Keypad.h>

/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>
#include "Keyboard.h"
#include <SPI.h>
#include <SD.h>

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

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.write(0xFE);
  mySerial.write(0x58);
  mySerial.write(0xFE);
  mySerial.write(0x48);
}

void loop() { // run over and over
  char key = numpad.getKey();

  if (key){
    mySerial.print(key);
    Keyboard.print(key);

    if (key == 'D'){
      listSD();
    }
  }
}


void listSD()
{


  root = SD.open("/");

  printDirectory(root, 0);

  Keyboard.println("done!");
}


void printDirectory(File dir, int numTabs) {
  // Begin at the start of the directory
  dir.rewindDirectory();

  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Keyboard.print('\t');   // we'll have a nice indentation
     }
     // Print the 8.3 name
     Keyboard.print(entry.name());
     // Recurse for directories, otherwise print the file size
     if (entry.isDirectory()) {
       Keyboard.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Keyboard.print("\t\t");
       Keyboard.print(entry.size(), DEC);
       Keyboard.print('\n');
     }
     entry.close();
   }
}
