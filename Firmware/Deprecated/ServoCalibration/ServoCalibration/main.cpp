/*
 Name:		ESP32MIDI.ino
 Created:	5/6/2022 6:17:48 PM
 Author:	Dr. G
*/

#include <MIDI.h>
#include <Arduino.h>
#include <Math.h>
//#include <Control_Surface.h>
#include "configuration.h"
#include "ServoHandler.h"

//for STM32 boards, Serial is Serial1
//#ifdef USING_STM32
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
//#else
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
//#endif

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

//System clock
void GetTicks(void)
{
	LastMillisecondTicks = MillisecondTicks;
	LastMicrosecondTicks = MicrosecondTicks;

	MillisecondTicks = millis();
	MicrosecondTicks = micros();
}


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

unsigned int selectedServo{};
bool hasClicked{};

void setup() {


    Serial.begin(9600);
    //TEMP serves as our button pin
    //pinMode(PB1, INPUT_PULLUP);
    //pinMode(PC13, OUTPUT);
    //tie callbacks to midi events
    //MIDI.setHandleNoteOn(onNoteOn);
    //MIDI.setHandleNoteOff(onNoteOff);
    //MIDI.begin();

    //old control surface stuff
    //MIDI_SERIAL.begin(); // Initialize midi interface
    //MIDI_SERIAL.setCallbacks(callback);
    //MIDI_BLE.begin(); // Initialize midi interface
    //MIDI_BLE.setCallbacks(callback);


    selectedServo = 29;
    SetupServo();
    pinMode(PA0, INPUT);//potentiometer
    pinMode(PC15, INPUT_PULLUP);//"button"

    
    //pinMode(PB2, OUTPUT);
    //pinMode(PA14, OUTPUT);

}


void loop() {

    GetTicks();


    //an effort to conserve cycles by only calling this once per time ammount
    if( MillisecondTicks % 500 == 0 && MillisecondTicks != LastMillisecondTicks)
    {




    //arduino IDE-s tab-spacing is nasty
    if(digitalRead(PC15) == 0 && hasClicked == false)
    {
        hasClicked = true;

        ++selectedServo;

        if(selectedServo == SERVO_COUNT)
        {
            selectedServo = 0;  
        }

        
    }
    else if(digitalRead(PC15) == 1)
    {
        hasClicked = false;
    }

      
        Serial.print("selectedServo: ");
        Serial.print(selectedServo);
        Serial.print(" Button: ");
        Serial.print(digitalRead(PC15));
        Serial.print(" Potentiometer: ");
        Serial.println(map(analogRead(PA0), 0, 4095, 900, 1600));

        
    }




    SMT[selectedServo].currentPPM = map(analogRead(PA0), 0, 4095, 900, 1600);

    
    //perform servo actions
    //static bool pressedButton = false;


    ServoBackend(&SMT[selectedServo]);
    //AllServoSweep();
    //AllServoDrive();
    //WaferServoDrive();




}
