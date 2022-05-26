#include <Arduino.h>
#include "configuration.h"
#include "ServoHandler.h"
//#include <Control_Surface.h>


//Servo Map Table
SERVO_ID SMT[] =
{
	//pin, note, minRange, maxRange, currentPPM, requestNote, goForward
	{4, 0, 500, 2000, 500, false, false},
	{PB7, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
	{4, 60, 500, 2000, 500, false, false},
};

SERVO_ID CONTINUOUS_SERVO =
	//pin, note, minRange, maxRange, currentPPM, requestNote, goForward
	{CONTINUOUS_SERVO_PIN, 0, CONTINUOUS_REST_ANGLE, CONTINUOUS_ACTIVE_ANGLE, CONTINUOUS_REST_ANGLE, false, false};


unsigned long MillisecondTicks{};
unsigned long MicrosecondTicks{};
unsigned long LastMillisecondTicks{};//previous values
unsigned long LastMicrosecondTicks{};

//unsigned char isFinished{};//use the char to store several booleans


//pinMode all the output pins
void SetupServo(void)
{
	//note servos
	for (int i = 0; i < SERVO_COUNT; ++i)
	{
		pinMode(SMT[i].pinNo, OUTPUT);
	}

	//wafer servo
	pinMode(CONTINUOUS_SERVO.pinNo, OUTPUT);

}

//send the correct pulses to the servo
void ServoBackend(SERVO_ID *tower)
{
	//global servo clock
	//every 20 Milliseconds, we will set all servo pins to HIGH
	//and then turn them off if the microsecond count is >= to the value prescribed

	//used to get the change in microseconds every 20 Milliseconds
	static unsigned long divyMicroseconds{};
	static unsigned long previousMicrosecondTick{};


	if (MillisecondTicks % 20 == 0 && MillisecondTicks != LastMillisecondTicks)// !(isFinished & SERVO_HIT_REFRESH))
	{
		//handy note: '|=' adds a binary flag, '&= ~' removes it

		//moved the for() loops outside this function

		previousMicrosecondTick = MicrosecondTicks;


		//set servo pin to HIGH every 20 milliseconds
		digitalWrite(tower->pinNo, true);
		
	}
	else
	{

		//get the passage of microseconds
		divyMicroseconds = MicrosecondTicks - previousMicrosecondTick;

		
		if (tower->currentPPM < divyMicroseconds)
		{
			digitalWrite(tower->pinNo, false);
		}
		

	}



}

//drives the note servos
void AllServoDrive(void)
{
	for(int i = 0; i < SERVO_COUNT; ++i)
	{
		ServoBackend(&SMT[i]);

	}

}

//drives the wafer spinner servo
void WaferServoDrive(void)
{

	static unsigned int waferTimeout{1000};
	//static bool waferSpeedSet{};

	for(int i = 0; i < SERVO_COUNT; ++i)
	{
		//if any servo is currently playing a note
		if(SMT[i].goForward == true)
		{
			waferTimeout = 0;	
		}	
	}

	//once every millisecond
	if (MillisecondTicks != LastMillisecondTicks)
	{
		++waferTimeout;
	}

	//this will only attach the servo if it needs to be run, which will save energy
	if(waferTimeout < CONTINUOUS_RUN_TIME)
	{
		digitalWrite(PC13, false);

		CONTINUOUS_SERVO.currentPPM = CONTINUOUS_SERVO.maxRange;
		ServoBackend(&CONTINUOUS_SERVO);
	}
	else
	{
		digitalWrite(PC13, true);
		CONTINUOUS_SERVO.currentPPM = CONTINUOUS_SERVO.minRange;
		ServoBackend(&CONTINUOUS_SERVO);
	}


}

//sweep each servo when a trigger variable is called
void SingleServoSweep(SERVO_ID *tower)
{


	if (tower->requestNote == true)
	{
		tower->requestNote = false;
		tower->goForward = true;
	}


	if (tower->goForward == true)
	{
		//add a value to its angle
		if (tower->currentPPM < tower->maxRange)
		{
			//ratio of PPM range to time (the *2 is the result of the need to devide this between forward and backward motion)
			//the function assumes it will be called every Millisecond
			tower->currentPPM += ((tower->maxRange - tower->minRange) * 2) / SERVO_PUSH_TIME;

		}
		else
		{

			tower->goForward = false;
		}


	}
	else if (tower->currentPPM > tower->minRange)
	{
		tower->currentPPM -= ((tower->maxRange - tower->minRange) * 2) / SERVO_PUSH_TIME;
	}



}

//runs the function above every 1 Millisecond, for all servos
void AllServoSweep(void)
{
	//every Millisecond
	if (MillisecondTicks != LastMillisecondTicks)
	{
		//runs the servo sweep function for each servo in the list
		for (int i = 0; i < SERVO_COUNT; ++i)
		{
			SingleServoSweep(&SMT[i]);
		}


	}


}
