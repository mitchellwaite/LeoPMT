# LeoPMT - (Arduino) Leonardo Password Manager Thingy

## What is LeoPMT?

LeoPMT is a device, based on an arduino leonardo (Atmega 32u4), that ~encrypts~ and stores your passwords for later retreival. It emulates a USB HID keyboard, so it can automatically enter passwords for you. It is compatible not only with computers, but also Android and iOS devices with the proper adapter.

This was designed and implemented at Hack Western, Nov 17-19 2017

## Hardware Required

- ~Arduino 101~ Arduino Leonardo (or equivalent Atmega 32u4 board)
- SD Card adapter
- Adafruit RGB character LCD

## Libraries Required

- Adafruit SD library: https://github.com/adafruit/SD
- Keypad library : (tbd)
- Crypto library : https://github.com/rweather/arduinolibs/tree/master/libraries/Crypto

## But, I though it was supposed to be encrypted?

Unfortunately, I originally planned to use an Intel based Arduino 101, which is a lot more powerful and has built in bluetooth. Turns out, the one I was provided with was broken. There weren't any more left, so i was stuck with an arduino leonardo that i brought. Leonardos do not have enough program space for all the goodies... just the SD card library takes up a significant portion.

## Future Improvements

- [ ] Update to an arduino that is more powerful (more program space, faster CPU, etc.) Any arduino with built in USB should work for the keyboard emulator; this includes the DUE, and any Atmel SAMD CPU boards.
- [ ] Add support for bluetooth keyboard emulation. This can be implemented with a BlueFruit EZ-KEY or an arduino with built in bluetooth, like the 101.
- [ ] Add calls in the appropriate places to the crypto functions, to encrypt data as it's written to and decrypt data as it's read from the SD card.
- [ ] Add a call to a PBKDF2 implementation, to properly generate an encryption key from the user's passcode.
- [ ] Add some hardware for a true random number generator, so we can generate strong master encryption keys. This would be encrypted with the key derived from the user's password and stored on the SD
