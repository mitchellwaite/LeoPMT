#include <Key.h>
#include <Keypad.h>
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
  clearLCD();

  char passcodeBuf[33] = {'\0'};

  SD.mkdir("psw");

  checkNewPasscode();

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
    getPasscode(checkPasscodeBuf, 33, "Enter Passcode:", false);
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
      
      clearLCDRed();
      mySerial.println("Wrong Passcode.");
      delay(1500);
    }
    else
    {
      clearLCDGreen();
      mySerial.println("Success.");
      delay(1500);
      break;
    }
  }
  
}

void loop() { // run over and over
  static int menuIdx = 0;
  char passName[9] = {'\0'};
  
  clearLCD();

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
      case 4:
        changePasscode();
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
  clearLCDRed();
  mySerial.print("not implemented");
  delay(1000);
}

void passwordMenu(){
  return;
}

void changePasscode(){
  SD.remove("mkey.dat");
  checkNewPasscode();
}

void checkNewPasscode(){
  if (!SD.exists("mkey.dat")){
    //Set up a new encryption key
    byte newPasscodeBuf[33] = {'\0'};
    getPasscode(newPasscodeBuf, 33, "New Passcode:",false);
    myFile = SD.open("mkey.dat", FILE_WRITE);
    myFile.write(newPasscodeBuf, 33);
    myFile.close();
  }
}

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
    clearLCDRed();
    mySerial.println("Pswrd file");
    mySerial.print("is empty!");
    delay(1500);

    myFile.close();
    return;
  }
  else
  {
    clearLCD();
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

      clearLCD();
      mySerial.println(pswFile.name());
      pswFile.read(filePsw, max(pswFile.available(),32));
      mySerial.print("^(*) X(#) >(D)"); 
      
    }
    
  }
  
  pswFile.close();
  myFile.close();
  
}

void makeNewPassword()
{
  char passName[9] = {'\0'};
  char actualPass[11] = {'\0'};
  char * actualDest = NULL;
  
  bool continueLoop = false;
  
  do
  {
    char * dest = "/PSW/";
    getCodeT9(passName,9,"Enter Name:");
    strcat(dest,passName);
    
    if(SD.exists(dest))
    {
      continueLoop = true;
      clearLCDRed();
      mySerial.println("Pswrd exists!");
      mySerial.print("Try Again.");
      delay(1500);
    }
    else
    {
      actualDest = dest;
    }
  }while(continueLoop);

  clearLCDGreen();
  mySerial.println("Your Name:");
  mySerial.print(passName);
  delay(2000);

  for(int i = 0;i<10;i++)
  {
    actualPass[i] = random(33,126);
  }

  clearLCDRed();

  mySerial.print("N: ");
  mySerial.println(passName);

  mySerial.print("P: ");
  mySerial.print(actualPass);

  delay(5000);

  myFile = SD.open(actualDest, FILE_WRITE);
  myFile.write(actualPass);
  myFile.close();
}

void getCodeT9(char * keyBuffer, size_t bufSz, String message)
{
  int keyCount = 0;
  char currentChar = ' ';
  char currentKey = ' ';

  while(true)
  {
    
    clearLCD();
    mySerial.println(message);

    for(int i = 0;i<keyCount;i++)
    {
      mySerial.print(keyBuffer[i]);
    }

    mySerial.print(currentChar);
    
    char key = numpad.getKey();

    while(!key)
    {
      key = numpad.getKey();
    }
  
    if (key == '#'){
      //Commit a waiting character
      if(currentChar != ' ')
      {
        keyBuffer[keyCount] = currentChar;
        currentChar = ' ';
        currentKey = ' ';
        keyCount = keyCount + 1; 
      }
      
      if(keyCount < bufSz)
      {
        keyBuffer[keyCount] = '\0';
      }
      else
      {
        keyBuffer[bufSz - 1] = '\0';
      }
      break;
    }
    else if(key == 'D')
    {
      //backspace
      if(keyCount > 0)
      {
        keyBuffer[keyCount] = '\0';
        currentChar = ' ';
        currentKey = ' ';
        keyCount = keyCount - 1;
      }
    }
    else if(key == '*')
    {
      if(currentChar != ' ')
      {
        keyBuffer[keyCount] = currentChar;
        currentChar = ' ';
        currentKey = ' ';
        keyCount = keyCount + 1; 
      }
    }
    else if(key >= 48 && key <= 57)
    {
      if(key != currentKey)
      {
        currentKey = key;
        currentChar = key;
      }
      else
      {
        switch(currentKey)
        {
          case '2':
            if (currentChar == '2')
            {
              currentChar = 'A';
            }
            else if(currentChar == 'A')
            {
              currentChar = 'B';
            }
            else if(currentChar == 'B')
            {
              currentChar = 'C';
            }
            else if(currentChar == 'C')
            {
              currentChar = '2';
            }
          case '3':
            if (currentChar == '3')
            {
              currentChar = 'D';
            }
            else if(currentChar == 'D')
            {
              currentChar = 'E';
            }
            else if(currentChar == 'E')
            {
              currentChar = 'F';
            }
            else if(currentChar == 'F')
            {
              currentChar = '3';
            }
          case '4':
            if (currentChar == '4')
            {
              currentChar = 'G';
            }
            else if(currentChar == 'G')
            {
              currentChar = 'H';
            }
            else if(currentChar == 'H')
            {
              currentChar = 'I';
            }
            else if(currentChar == 'I')
            {
              currentChar = '4';
            }
            break;
          case '5':
            if (currentChar == '5')
            {
              currentChar = 'J';
            }
            else if(currentChar == 'J')
            {
              currentChar = 'K';
            }
            else if(currentChar == 'K')
            {
              currentChar = 'L';
            }
            else if(currentChar == 'L')
            {
              currentChar = '5';
            }
            break;
          case '6':
            if (currentChar == '6')
            {
              currentChar = 'M';
            }
            else if(currentChar == 'M')
            {
              currentChar = 'N';
            }
            else if(currentChar == 'N')
            {
              currentChar = 'O';
            }
            else if(currentChar == 'O')
            {
              currentChar = '6';
            }
            break;
          case '7':
            if (currentChar == '7')
            {
              currentChar = 'P';
            }
            else if(currentChar == 'P')
            {
              currentChar = 'R';
            }
            else if(currentChar == 'R')
            {
              currentChar = 'S';
            }
            else if(currentChar == 'S')
            {
              currentChar = '7';
            }
            break;
          case '8':
            if (currentChar == '8')
            {
              currentChar = 'T';
            }
            else if(currentChar == 'T')
            {
              currentChar = 'U';
            }
            else if(currentChar == 'U')
            {
              currentChar = 'V';
            }
            else if(currentChar == 'V')
            {
              currentChar = '8';
            }
            break;
          case '9':
            if (currentChar == '9')
            {
              currentChar = 'W';
            }
            else if(currentChar == 'W')
            {
              currentChar = 'X';
            }
            else if(currentChar == 'X')
            {
              currentChar = 'Y';
            }
            else if(currentChar == 'Y')
            {
              currentChar = '9';
            }
            break;
        }
      }
    }
    
  }
   
  return;
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
        if(keyCount < bufSz)
        {
          keyBuffer[keyCount] = '\0';
        }
        else
        {
          keyBuffer[bufSz - 1] = '\0';
        }
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
          keyBuffer[keyCount] = key;
          keyCount++;
        }
      }
    }
  }
   
  return;
}

void clearLCD()
{
  mySerial.write(0xFE);
  mySerial.write(0x58);
  mySerial.write(0xFE);
  mySerial.write(0x48);

  mySerial.write(0xFE);
  mySerial.write(0xD0);

  //Colours
  mySerial.write(0xFF);
  mySerial.write(0xFF);
  mySerial.write(0xFF); 
}

void clearLCDGreen(){
  clearLCD();

  mySerial.write(0xFE);
  mySerial.write(0xD0);

  //Colours
  mySerial.write((byte)0x00);
  mySerial.write(0xFF);
  mySerial.write((byte)0x00); 
}

void clearLCDRed(){
  clearLCD();

  mySerial.write(0xFE);
  mySerial.write(0xD0);

  //Colours
  mySerial.write(0xFF);
  mySerial.write((byte)0x0);
  mySerial.write((byte)0x0); 
}
