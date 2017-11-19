# LeoPMT - (Arduino) Leonardo Password Manager Thingy

## What is LeoPMT?

LeoPMT is a device, based on an arduino leonardo (Atmega 32u4), that stores your passwords for later retreival. It emulates a USB HID keyboard, so it can automatically enter passwords for you. It is compatible not only with computers, but also Android and iOS devices with the proper adapter. This device is **completely isolated from the internet**, so there is no risk of it being remotely compromised.

This was designed and implemented at Hack Western, Nov 17-19 2017

## Screenshots and Images

[Click here for images of LeoPMT](https://github.com/mitchellwaite/LeoPMT/blob/master/IMAGES.MD)

## Controls

Since we're stuck with a character LCD, it's a bit difficult to display the actions for each button. Here is a summary:

### Main Screen

- Advance Item = D Key
- Select Item = Pound Key

### Number/Passcode Entry Screen

- Number Entry = 0-9
- Backspace = D Key
- Confirm = Pound Key

### Text Entry Screen / T9 Dialer (Works like texting on a flip phone)

- Number/text entry = 0-9
- Confirm character = Star key
- Confirm string = Pound Key
- Backspace = D Key 

## Getting Started

- Format the SD card, and insert it in to the SD card slot on the shield. Plug in the USB to your phone or computer, and you'll be ready to go!

### First Boot or Clean Wipe

On the first boot after inserting a fresh SD card, you will be asked to enter a passcode. This is the master device passcode. Use the number pad (0-9) to enter an access code, then hit pound for confirmation. LeoPMT will then continue the regular boot sequence.

### Regular boot

On subsequent powerups, LeoPMT will ask for the passcode entered on first boot.

## Menu Items

### 1) Passwords

Shows a list of passwords, by using the names provided at password creation. Allows the user to send the password text to a computer.

- Press D to advance through the list of passwords
- Press #/Pound key to send the password keystrokes to the connected computer
- Press */Star to go to the main menu

### 2) New Pswrd (New Password)

Guides the user through creation of a new password. See the T9 dialer section above for controls for name entry. The text of each password is randomly generated from Uppercase, Lowercase, Symbols, and Numbers. 

### 3) Chg Pswrd (Change Password)

Shows a list of passwords, by using the names provided at password creation. Allows the user to change the password text of an entry.

- Press D to advance through the list of passwords
- Press #/Pound key to change the password text. A confirmation screen will appear.
- Press */Star to go to the main menu

Ensure you enter the old password in a change password form on your computer before you change or delete the password on LeoPMT

### 4) Del Pswrd (Delete Password)

Shows a list of passwords, by using the names provided at password creation. Allows the user to delete password entries.

- Press D to advance through the list of passwords
- Press #/Pound key to change the password text. A confirmation screen will appear.
- Press */Star to go to the main menu

Ensure you enter the old password in a change password form on your computer, or some sort of contingency plan if necessary, before you delete the password on LeoPMT

### 5) Chg Passscode

Guides the user through the change of the main device passcode. Use the number pad (0-9) to enter an access code, then hit pound for confirmation.

### 6) Chg Pswd Len

Allows the user to change the length of the password generated. This value is stored in ROM, so it persists across power cycles and SD cards.

### 7) Clean Wipe

Deletes all information from the SD Card. This is a destructive operation, so confirmation will be required. After contents have been deleted, you must power cycle LeoPMT.

## Video Demonstration

## Hardware Required

- ~Arduino 101~ Arduino Leonardo (or equivalent Atmega 32u4 board)
- SD Card shield/breakout board
- FAT32 formatted SD card (SD/SDHC)
- Adafruit RGB character LCD

## Libraries Required

You can get the keypad library from the library manager feature in the arduino IDE.

- Adafruit SD library: https://github.com/adafruit/SD
- Keypad library : http://playground.arduino.cc/Code/Keypad
- ~Crypto library~ : https://github.com/rweather/arduinolibs/tree/master/libraries/Crypto



## But, I though it was supposed to be encrypted?

Unfortunately, I originally planned to use an Intel based Arduino 101, which is a lot more powerful and has built in bluetooth. Turns out, the one I was provided with was broken. There weren't any more left, so i was stuck with an arduino leonardo that i brought. Leonardos do not have enough program space for all the goodies... just the SD card library takes up a significant portion.

## Future Improvements

- [ ] Update to an arduino that is more powerful (more program space, faster CPU, etc.) Any arduino with built in USB should work for the keyboard emulator; this includes the DUE, and any Atmel SAMD CPU boards.
- [ ] Add support for bluetooth keyboard emulation. This can be implemented with a BlueFruit EZ-KEY or an arduino with built in bluetooth, like the 101.
- [ ] Add calls in the appropriate places to the crypto functions, to encrypt data as it's written to and decrypt data as it's read from the SD card.
- [ ] Add a call to a PBKDF2 implementation, to properly generate an encryption key from the user's passcode.
- [ ] Add some hardware for a true random number generator, so we can generate strong master encryption keys. This would be encrypted with the key derived from the user's password and stored on the SD
