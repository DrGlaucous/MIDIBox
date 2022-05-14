/*
 Name:		ESP32MIDI.ino
 Created:	5/6/2022 6:17:48 PM
 Author:	Dr. G
*/

#include <Arduino.h>
#include <Control_Surface.h>
#include "ServoHandler.h"


HardwareSerialMIDI_Interface MIDI_SERIAL = Serial;

BluetoothMIDI_Interface MIDI_BLE;

// Custom MIDI callback that prints incoming messages.
struct MyMIDI_Callbacks : FineGrainedMIDI_Callbacks<MyMIDI_Callbacks> {
    // Note how this ^ name is identical to the argument used here ^

    // Function that is called whenever a MIDI Note Off message is received. (This is not important when playing notes of a fixed length (I.E a music box))
    //void onNoteOff(Channel channel, uint8_t note, uint8_t velocity, Cable cable) {
    //    digitalWrite(5, 0);
    //}

    // Function that is called whenever a MIDI Note On message is received.
    void onNoteOn(Channel channel, uint8_t note, uint8_t velocity, Cable cable) {
        
        for (int i = 0; i < SERVO_COUNT; ++i)
        {
            if (SMT[i].noteNo == note)
            {
                SMT[i].requestNote = true;
            }
        }

    }

} callback;





// the setup function runs once when you press reset or power the board
void setup() {

    SetupServo();


    //TEMP serves as our button pin
    pinMode(5, INPUT_PULLUP);


    MIDI_SERIAL.begin(); // Initialize midi interface
    MIDI_SERIAL.setCallbacks(callback);

    MIDI_BLE.begin(); // Initialize midi interface
    MIDI_BLE.setCallbacks(callback);
}

// the loop function runs over and over again until power down or reset
void loop() {

    GetTicks();


    MIDI_SERIAL.update(); // Update the Control Surface
    MIDI_BLE.update();


    //perform servo actions
    static bool pressedButton = false;
    //bits of trial code
    if (digitalRead(5) == 0)
    {
        if (pressedButton == false)
        {
        
            SMT[1].requestNote = true;


            pressedButton = true;
        }


    }
    else
    {
        pressedButton = false;
    }


    AllServoSweep();
    AllServoDrive();



}

