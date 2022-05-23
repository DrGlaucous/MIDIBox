/*
 Name:		ESP32MIDI.ino
 Created:	5/6/2022 6:17:48 PM
 Author:	Dr. G
*/

#include <MIDI.h>
#include <Arduino.h>
//#include <Control_Surface.h>
#include "configuration.h"
#include "ServoHandler.h"

//for STM32 boards, Serial is Serial1
#ifdef USING_STM32
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#else
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
#endif

/*
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
*/



// Function that is called whenever a MIDI Note On message is received.
void onNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {

    //digitalWrite(PC13, false);//this LED is inverted: "false" is actually "on"

    for (int i = 0; i < SERVO_COUNT; ++i)
    {
        if (SMT[i].noteNo == pitch)
        {
            SMT[i].requestNote = true;
        }
    }

}
void onNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    //digitalWrite(PC13, true);

}



// the setup function runs once when you press reset or power the board
void setup() {

    SetupServo();


    //TEMP serves as our button pin
    pinMode(PB1, INPUT_PULLUP);
    pinMode(PC13, OUTPUT);
    //tie callbacks to midi events
    MIDI.setHandleNoteOn(onNoteOn);
    MIDI.setHandleNoteOff(onNoteOff);
    MIDI.begin();

    //old control surface stuff
    //MIDI_SERIAL.begin(); // Initialize midi interface
    //MIDI_SERIAL.setCallbacks(callback);
    //MIDI_BLE.begin(); // Initialize midi interface
    //MIDI_BLE.setCallbacks(callback);
}

// the loop function runs over and over again until power down or reset
void loop() {

    GetTicks();

    MIDI.read();


    //old control surface stuff
    //MIDI_SERIAL.update(); // Update the Control Surface
    //MIDI_BLE.update();
    
    //perform servo actions
    static bool pressedButton = false;
    //bits of trial code

    if (digitalRead(PB1) == ON_STATE)
    {
        if (pressedButton == false)
        {
        
            onNoteOn(0, 60, 120);

            pressedButton = true;
        }


    }
    else
    {
        pressedButton = false;
    }


    AllServoSweep();
    AllServoDrive();
    WaferServoDrive();




    //onboard LED if ANY servos are active
    for(int i = 0; i <= SERVO_COUNT; ++i)
    {
        
        if(SMT[i].goForward == true)
        {
            //digitalWrite(PC13, true);
            break;
        }
        else if(i == SERVO_COUNT)
        {
            //digitalWrite(PC13, false);
        }

    }





}

