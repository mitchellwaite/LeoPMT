#include "libpmt.h"

void clearLCD(SoftwareSerial mySerial)
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

void clearLCDGreen(SoftwareSerial mySerial){
  clearLCD(mySerial);

  mySerial.write(0xFE);
  mySerial.write(0xD0);

  //Colours
  mySerial.write((unsigned char)0x00);
  mySerial.write(0xFF);
  mySerial.write((unsigned char)0x00); 
}

void clearLCDRed(SoftwareSerial mySerial){
  clearLCD(mySerial);

  mySerial.write(0xFE);
  mySerial.write(0xD0);

  //Colours
  mySerial.write(0xFF);
  mySerial.write((unsigned char)0x0);
  mySerial.write((unsigned char)0x0); 
}

void getCodeT9(SoftwareSerial mySerial, Keypad numpad, char * keyBuffer, size_t bufSz, String message)
{
  int keyCount = 0;
  char currentChar = ' ';
  char currentKey = ' ';

  while(true)
  {
    
    clearLCD(mySerial);
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
    else if(key == '*')
    {
      //backspace
      if(keyCount > 0)
      {
        if(currentChar != ' ')
        {
          currentChar = ' ';
          currentKey = ' ';
        }
        else
        {
          keyBuffer[keyCount] = '\0';
          currentChar = ' ';
          currentKey = ' ';
          keyCount = keyCount - 1;
        }
      }
    }
    else if(key == 'D')
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
        if(currentKey != ' ')
        {
          keyBuffer[keyCount] = currentChar;
          keyCount = keyCount + 1; 
        }
        
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

void getPasscode(SoftwareSerial mySerial, Keypad numpad, char * keyBuffer, size_t bufSz, String message, bool showChars)
{
  int keyCount = 0;
  
  clearLCD(mySerial);

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
