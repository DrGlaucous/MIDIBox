#pragma once

#define SERVO_COUNT 30
#define SERVO_PUSH_TIME 500//in Milliseconds (1K == 1 second)

typedef struct SERVO_ID
{
	const int pinNo;//servo pin
	const int noteNo;//servo's note
	int minRange;//will go between these values when pushing a note
	int maxRange;
	int currentPPM;//where the servo currently is in the sweep (we would only need this if we want to have the servo travel slower than max speed)
	bool requestNote;//initally sent to start the servo playing a note
	bool goForward;//direction variable
	//ints are 2 bytes with arduinos

}SERVO_ID;

//to have several booleans share a char
/*
enum {
	SERVO_HIT_REFRESH = 1,
	INCREMENT_HIT_REFRESH = 2,



};
*/


extern SERVO_ID SMT[SERVO_COUNT];
extern unsigned long MillisecondTicks;
extern unsigned long MicrosecondTicks;
extern unsigned long LastMillisecondTicks;
extern unsigned long LastMicrosecondTicks;
//extern unsigned char isFinished;

void SetupServo(void);
void AllServoDrive(void);
void AllServoSweep(void);
void GetTicks(void);