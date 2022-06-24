//Configuration
//this program was built using PlatformIO.
//with some tweaks, it COULD be built using the arduino IDE enviroment. All libraries used in this project are arduino


//this is not really needed, but I have it here for my personal reference. the arduino IDE doesn't like it here, but platformIO doesn't mind
#ifndef USING_STM32
// Note: this needs to match with the PIN_MAP array in board.cpp
//STM32 pin mapping
enum {
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13,PA14,PA15,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13,PB14,PB15,
	PC13, PC14,PC15
};
#endif



//the state of a pin when it is "high" or "clicked" (nothing actually uses this in the code, but its useful for debugging)
#define ON_STATE 0

//number of notes on the music box
#define SERVO_COUNT 30
#define SERVO_PUSH_TIME 360//in Milliseconds (1K == 1 second)


#define CONTINUOUS_REST_ANGLE 1500//the angle the continuous rotation servo will be set to "at rest"
#define CONTINUOUS_ACTIVE_ANGLE 700//the angle it uses when rotating (i believe the more different it is from the rest angle, the faster the servo will go)
#define CONTINUOUS_SERVO_PIN PA14//PA14//pin for this servo (I use PC13 for debugging)
#define CONTINUOUS_RUN_TIME 500//time in milliseconds the servo will spin





#define SMOOTH_SWEEP//sweeps the servo at a controlled rate, giving steps of angles. if not defined, servos will sweep with only the min and max angles (this is faster, but less well-timed)

//#define WAIT_UNTIL_DONE//will not try to play another note on the servo until it has retracted fully
