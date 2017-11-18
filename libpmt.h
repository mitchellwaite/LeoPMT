//Leonardo Password Management Thingy Library Functions
#include <SoftwareSerial.h>
#include <Key.h>
#include <Keypad.h>

//LCD Control
void clearLCD(SoftwareSerial mySerial);
void clearLCDGreen(SoftwareSerial mySerial);
void clearLCDRed(SoftwareSerial mySerial);

//Input Functions
void getCodeT9(SoftwareSerial mySerial, Keypad numpad, char * keyBuffer, size_t bufSz, String message);
void getPasscode(SoftwareSerial mySerial, Keypad numpad, char * keyBuffer, size_t bufSz, String message, bool showChars);
