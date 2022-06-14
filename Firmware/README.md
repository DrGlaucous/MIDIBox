### Firmware
---
This folder contains the firmware that drives the MIDI controlled music box. It was originally designed around the ESP32 development board, but changed to work with the STM32 bluepill when I found that the ESP32 did not have the required amount of GPI/O pins for the project (31 + 2 for RX and TX, 33 total). Of course, you can use whatever board you wish, so long as it is compatible with the arduino `MIDI.h` library.

The STM32 only has 15 PWM pins, but needs to drive 31 servos, so the software is designed to do this with bit banging. To conserve CPU cycles and minimize servo chatter, servos are only attached when they are needed to push the note fingers.

With this project, I tried several different development platforms when programming the firmware. I started with the arduino IDE, then moved to visualMicro and eventually settled on PlatformIO to do the bulk of my programming.

If, for some reason, you *really* don't want to use PlatformIO to upload the firmware *(though I highly suggest you do, it is much better than the stock IDE)*, you should be able to use the standard arduino IDE and open the main.cpp file in the source directory. You will need to find and remove any `#ifdef STM32` preprocessor statements or you will run into compile errors. This is only defined in the platformIO config file.

---
The final firmware is in the "PlatformIO" folder, and the old stuff is in the "Deprecated" folder.