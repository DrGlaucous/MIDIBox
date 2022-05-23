//Configuration
//this program was build using PlatformIO.
//with some tweaks, it COULD be built using the arduino IDE enviroment. All libraries used in this project are arduino


#ifndef USING_STM32
// Note: this needs to match with the PIN_MAP array in board.cpp
//STM32 pin mapping
enum {
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13,PA14,PA15,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13,PB14,PB15,
	PC13, PC14,PC15
};
#endif



//the state of a pin when it is "high" or "clicked"
#define ON_STATE 0

//number of notes on the music box
#define SERVO_COUNT 30
#define SERVO_PUSH_TIME 500//in Milliseconds (1K == 1 second)


#define CONTINUOUS_REST_ANGLE 1500//the angle the continuous rotation servo will be set to "at rest"
#define CONTINUOUS_ACTIVE_ANGLE 700//the angle it uses when rotating (i believe the more different it is from the rest angle, the faster the servo will go)
#define CONTINUOUS_SERVO_PIN PB6//pin for this servo
#define CONTINUOUS_RUN_TIME 1000//time in milliseconds the servo will spin
