#include <Arduino.h>
#include "ServoHandler.h"
//#include <Control_Surface.h>


//Servo Map Table
SERVO_ID SMT[] =
{
	//pin, note, minRange, maxRange, currentPPM, requestNote, goForward
	{4, 0, 500, 2000, 500, false, false},
	{19, 60, 500, 2000, 500, false, false},
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


unsigned long MillisecondTicks{};
unsigned long MicrosecondTicks{};
unsigned long LastMillisecondTicks{};//previous values
unsigned long LastMicrosecondTicks{};

//unsigned char isFinished{};//use the char to store several booleans


//pinMode all the output pins
void SetupServo(void)
{
	for (int i = 0; i < SERVO_COUNT; ++i)
	{
		pinMode(SMT[i].pinNo, OUTPUT);
	}
}

//drives the servos
void AllServoDrive(void)
{
	//global servo clock
	//every 20 Milliseconds, we will set all servo pins to HIGH
	//and then turn them off if the microsecond count is >= to the value prescribed

	//used to get the change in microseconds every 20 Milliseconds
	static unsigned long divyMicroseconds{};//were static locals, made globals for debugging
	static unsigned long previousMicrosecondTick{};


	if (MillisecondTicks % 20 == 0 && MillisecondTicks != LastMillisecondTicks)// !(isFinished & SERVO_HIT_REFRESH))
	{

		previousMicrosecondTick = MicrosecondTicks;
		//divyMicroseconds = 0;//reset the PPM microseconds counter

		//I found a better way to do this
		//isFinished |= SERVO_HIT_REFRESH;

		//set servo pins to HIGH
		for (int i = 0; i < SERVO_COUNT; ++i)//i tried to use sizeof(), but that is the memory size in bytes, not entries in the array
		{
			//all servos will always be enabled
			digitalWrite(SMT[i].pinNo, true);
			
		}
		
	}
	else
	{
		//only remove this flag if the number of Milliseconds has incremented
		//if(MillisecondTicks % 20 != 0 && (isFinished & SERVO_HIT_REFRESH))
		//	isFinished &= ~SERVO_HIT_REFRESH;

		//get the passage of microseconds
		divyMicroseconds = MicrosecondTicks - previousMicrosecondTick;

		
		for (int i = 0; i < SERVO_COUNT; ++i)
		{
			if (SMT[i].currentPPM < divyMicroseconds)
			{
				digitalWrite(SMT[i].pinNo, false);
			}

		}
		



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

void GetTicks(void)
{
	LastMillisecondTicks = MillisecondTicks;
	LastMicrosecondTicks = MicrosecondTicks;

	MillisecondTicks = millis();
	MicrosecondTicks = micros();
}
