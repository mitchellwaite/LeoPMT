#include <Key.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include "Keyboard.h"
#include <SPI.h>
#include <SD.h>

#include "libpmt.h"

#define PSW_LEN 10

SoftwareSerial mySerial(A1, A0); // RX, TX

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns

char numberKeys[ROWS][COLS] = {
    { '1','2','3', 'A' },
    { '4','5','6', 'B' },
    { '7','8','9', 'C' },
    { '*','0','#', 'D' }
};

byte colPins[COLS] = {A2, A3, A4, A5}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {3, 4, 5, 6}; //connect to the column pinouts of the keypad
Keypad numpad( makeKeymap(numberKeys), rowPins, colPins, sizeof(rowPins), sizeof(colPins) );
File myFile;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 10;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  randomSeed(analogRead(A1));
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on Arduino Uno boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(SS, OUTPUT);
  SD.begin(10,11,12,13);

  Keyboard.begin();

  // Set up the LCD screen
  mySerial.begin(9600);
  clearLCD(mySerial);

  char passcodeBuf[33] = {'\0'};

  SD.mkdir("psw");

  checkNewPasscode();

  delay(250);

  myFile = SD.open("mkey.dat");

  int i = 0;
  while(myFile.available())
  {
    passcodeBuf[i] = myFile.read();
    Serial.print(passcodeBuf[i]);
    i = i+1;
  } 

  Serial.println("");
  
  myFile.close();

  Serial.print("passcode from file: ");

  for(int i = 0;i<33;i++)
  {
    Serial.print(passcodeBuf[i]);
  }

  char checkPasscodeBuf[33] = {'\0'};

  while(true)
  {
    getPasscode(mySerial, numpad, checkPasscodeBuf, 33, "Enter Passcode:", false);
    if(strcmp(checkPasscodeBuf, passcodeBuf))
    {
      Serial.print("entered code: ");
      for(int i = 0;i<33;i++)
      {
        Serial.print(checkPasscodeBuf[i]);
      }
      Serial.println("");

      Serial.print("expected code: ");
      for(int i = 0;i<33;i++)
      {
        Serial.print(passcodeBuf[i]);
      }
      Serial.println("");
      
      clearLCDRed(mySerial);
      mySerial.println("Wrong Passcode.");
      delay(1500);
    }
    else
    {
      clearLCDGreen(mySerial);
      mySerial.println("Success.");
      delay(1500);
      break;
    }
  }
  
}

void loop() { // run over and over
  static int menuIdx = 0;
  char passName[9] = {'\0'};
  
  clearLCD(mySerial);

  switch(menuIdx)
  {
    case 0:
      mySerial.println("1) Passwords");
      break;
    case 1:
      mySerial.println("2) New Pswrd");
      break;
    case 2:
      mySerial.println("3) Chg Pswrd");
      break;
    case 3:
      mySerial.println("4) Del Pswrd");
      break;
    case 4:
      mySerial.println("5) Chg Pscd");
      break;
    case 5:
      mySerial.println("6) Clean Wipe");
      break;
    default:
      mySerial.print(menuIdx);
      mySerial.println("< Error");
      break;
  }

  mySerial.print("X (#), > (D)");

  char key = numpad.getKey();

  while(!key)
  {
    key = numpad.getKey();
  }
  
  if(key == 'D')
  {
    menuIdx = (menuIdx + 1) % 6;    
  }
  else if(key == '#')
  {
    switch (menuIdx)
    {
      case 0:
        readPasswords();
        break;
      case 1:
        makeNewPassword();
        break;
      case 2:
        changePassword();
        break;
      case 3:
        deletePassword();
        break;
      case 4:
        changePasscode();
        break;
      case 5:
        cleanWipe();
        break;
      default:
        notImplemented();
        break;
    }

    delay(100);
  }
  
  
}

void notImplemented()
{
  clearLCDRed(mySerial);
  mySerial.print("not implemented");
  delay(1000);
}



void checkNewPasscode(){
  if (!SD.exists("mkey.dat")){
    //Set up a new encryption key
    byte newPasscodeBuf[33] = {'\0'};
    getPasscode(mySerial, numpad, newPasscodeBuf, 33, "New Passcode:",false);
    myFile = SD.open("mkey.dat", FILE_WRITE);
    myFile.write(newPasscodeBuf, 33);
    myFile.close();
  }
}

//Option 1 - Reads data files and can write password to emulated keyboard
void readPasswords()
{
  char key = '\0';
  char * fileName = "";
  char filePsw[32] = {'\0'};
  
  myFile = SD.open("/PSW");

  File pswFile = myFile.openNextFile();

  if(!pswFile)
  {
    //No files are present. Throw an error
    clearLCDRed(mySerial);
    mySerial.println("Pswrd file");
    mySerial.print("is empty!");
    delay(1500);

    myFile.close();
    return;
  }
  else
  {
    clearLCD(mySerial);
    mySerial.print("Pswrd: ");
    mySerial.println(pswFile.name());
    pswFile.read(filePsw, max(pswFile.available(),32));
    mySerial.print("^(*) X(#) >(D)"); 
  }

  while(true)
  {
        
    key = numpad.getKey();

    while(!key)
    {
      key = numpad.getKey();
    }

    if(key == '#')
    {
      Keyboard.print(filePsw);
    }
    else if(key == '*')
    {
      break;
    }
    else if(key == 'D')
    {
      //NEEEEEEEEXT
      pswFile = myFile.openNextFile();

      if(!pswFile)
      {
        myFile.rewindDirectory();
        pswFile = myFile.openNextFile();
      }

      clearLCD(mySerial);
      mySerial.print("Pswrd: ");
      mySerial.println(pswFile.name());
      pswFile.read(filePsw, max(pswFile.available(),32));
      mySerial.print("^(*) X(#) >(D)"); 
      
    }
    
  }
  
  pswFile.close();
  myFile.close();
  
}

//Option 2 - generates new password files
void makeNewPassword()
{
  char passName[9] = {'\0'};
  char actualPass[PSW_LEN + 1] = {'\0'};
  String dest;

  dest.concat("/PSW/");
  
  getCodeT9(mySerial, numpad, passName,9,"Enter Name:");

  dest.concat(passName);
  
  if(SD.exists(dest.c_str()))
  {
    clearLCDRed(mySerial);
    mySerial.println("Pswrd exists!");
    mySerial.print("Try Again.");
    delay(2500);
    return;
  }

  clearLCDGreen(mySerial);
  mySerial.println("Your Name:");
  mySerial.print(passName);
  delay(2000);

  for(int i = 0;i<PSW_LEN;i++)
  {
    actualPass[i] = random(33,126);
  }

  delay(1000);

  myFile = SD.open(dest.c_str(), FILE_WRITE);
  myFile.write(actualPass);
  myFile.close();

          
  clearLCDGreen(mySerial);
  mySerial.println("Password");
  mySerial.print("added.");
  delay(1500);
  
}

//Option 3 - Change Password
void changePassword()
{
  char key = '\0';
  char actualPass[PSW_LEN + 1] = {'\0'};
  
  myFile = SD.open("/PSW");

  File pswFile = myFile.openNextFile();

  if(!pswFile)
  {
    //No files are present. Throw an error
    clearLCDRed(mySerial);
    mySerial.println("Pswrd file");
    mySerial.print("is empty!");
    delay(1500);

    myFile.close();
    return;
  }
  else
  {
    clearLCD(mySerial);
    mySerial.print("Chg: ");
    mySerial.println(pswFile.name());
    mySerial.print("^(*) X(#) >(D)"); 
  }

  while(true)
  {
        
    key = numpad.getKey();

    while(!key)
    {
      key = numpad.getKey();
    }

    if(key == '#')
    {
        clearLCDRed(mySerial);
        
        mySerial.println("Change Pswrd!");
        mySerial.print("# Cont, * Back");

        char subKey = numpad.getKey();

        while(!subKey)
        {
          subKey = numpad.getKey();
        }

        if(subKey == '#')
        {
          String fdel;
          fdel.concat("/PSW/");
          fdel.concat(pswFile.name());
      
          pswFile.close();
          myFile.close();
          SD.remove(fdel.c_str());

          //Make new file here!
          for(int i = 0;i<PSW_LEN;i++)
          {
            actualPass[i] = random(33,126);
          }

          delay(1000);

          myFile = SD.open(fdel.c_str(), FILE_WRITE);
          myFile.write(actualPass);
          myFile.close();

          
          clearLCDRed(mySerial);
          mySerial.println("Password");
          mySerial.print("changed.");
          delay(1500);

          break;
        }
    }
    else if(key == '*')
    {
      break;
    }
    else if(key == 'D')
    {
      //NEEEEEEEEXT
      pswFile = myFile.openNextFile();

      if(!pswFile)
      {
        myFile.rewindDirectory();
        pswFile = myFile.openNextFile();
      }

      clearLCD(mySerial);
      mySerial.print("Chg: ");
      mySerial.println(pswFile.name());
      mySerial.print("^(*) X(#) >(D)"); 
      
    }
    
  }
  
  pswFile.close();
  myFile.close();
  
}

//Option 4 - Delete Password
void deletePassword()
{
  char key = '\0';
  //char * fileNameToRemove;
  char filePsw[32] = {'\0'};
  
  myFile = SD.open("/PSW");

  File pswFile = myFile.openNextFile();

  if(!pswFile)
  {
    //No files are present. Throw an error
    clearLCDRed(mySerial);
    mySerial.println("Pswrd file");
    mySerial.print("is empty!");
    delay(1500);

    myFile.close();
    return;
  }
  else
  {
    clearLCDRed(mySerial);
    mySerial.print("Del: ");
    mySerial.println(pswFile.name());
    mySerial.print("^(*) X(#) >(D)"); 
  }

  while(true)
  {
        
    key = numpad.getKey();

    while(!key)
    {
      key = numpad.getKey();
    }

    if(key == '#')
    {
        clearLCDRed(mySerial);
        
        mySerial.println("Destructive!");
        mySerial.print("# Cont, * Back");

        char subKey = numpad.getKey();

        while(!subKey)
        {
          subKey = numpad.getKey();
        }

        if(subKey == '#')
        {
          String fdel;
          fdel.concat("/PSW/");
          fdel.concat(pswFile.name());
      
          pswFile.close();
          myFile.close();
          SD.remove(fdel.c_str());
      
          clearLCDRed(mySerial);
          mySerial.println("Password");
          mySerial.print("deleted.");
          delay(1500);
          
          break;
        }
    }
    else if(key == '*')
    {
      break;
    }
    else if(key == 'D')
    {
      //NEEEEEEEEXT
      pswFile = myFile.openNextFile();

      if(!pswFile)
      {
        myFile.rewindDirectory();
        pswFile = myFile.openNextFile();
      }

      clearLCDRed(mySerial);
      mySerial.print("Del: ");
      mySerial.println(pswFile.name());
      mySerial.print("^(*) X(#) >(D)"); 
      
    }
    
  }
  
  pswFile.close();
  myFile.close();
  
}

//Option 5 - Change master passcode
void changePasscode(){
  SD.remove("mkey.dat");
  checkNewPasscode();

  clearLCDGreen(mySerial);
  mySerial.println("Passcode");
  mySerial.print("changed.");
  delay(1500);
}

//Option 6 - Clean Wipe
void cleanWipe() {
  clearLCDRed(mySerial);
  mySerial.println("Destructive!");
  mySerial.print("# Cont, * Back");

  char key = numpad.getKey();

  while(!key)
  {
    key = numpad.getKey();
  }

  if(key == '#')
  {
    //Delete all contents of PSW here...
    myFile = SD.open("/PSW");

    File pswFile = myFile.openNextFile();
    
    if(!pswFile)
    {
      myFile.close();
    }
    else
    {
      while(pswFile)
      {
          String fdel;
          fdel.concat("/PSW/");
          fdel.concat(pswFile.name());
          pswFile = myFile.openNextFile();
          SD.remove(fdel.c_str());
      }
    }

    SD.rmdir("/PSW");
    SD.remove("mkey.dat");

    pswFile.close();
    myFile.close();

    clearLCDRed(mySerial);
    mySerial.println("Cleaned! Power");
    mySerial.print("cycle now.");

    while(true){}//loop until power is turned off and on
  }
}

