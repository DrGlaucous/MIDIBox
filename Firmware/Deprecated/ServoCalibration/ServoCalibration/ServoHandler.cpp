#include <Arduino.h>
#include "configuration.h"
#include "ServoHandler.h"
//#include <Control_Surface.h>


//Servo Map Table
SERVO_ID SMT[] =
{
  //pin, note, minRange, maxRange, currentPPM, requestNote, goForward
  //main side (forwards)
  {PB12, 53, 900, 1500, 900, false, false},//lowest note, going up
  {PB13, 55, 1000, 1500, 1000, false, false},
  {PB14, 60, 1000, 1500, 1000, false, false},
  {PB15, 62, 1000, 1500, 1000, false, false},
  {PA8, 64, 1000, 1500, 1000, false, false},
  {PB2, 65, 1000, 1500, 1000, false, false},//PA11 USB- (unplug these two for USB serial)
  {PB2, 67, 1000, 1500, 1000, false, false},//PA12 USB+
  {PA15, 69, 1000, 1500, 1000, false, false},
  {PB3, 70, 1000, 1500, 1000, false, false},
  {PB4, 71, 1000, 1500, 1000, false, false},
  {PB5, 72, 1000, 1500, 1000, false, false},
  {PB6, 73, 1000, 1500, 1000, false, false},
  {PB7, 74, 1000, 1500, 1000, false, false},
  {PB8, 75, 1000, 1500, 1000, false, false},
  {PB9, 76, 1000, 1500, 1000, false, false},
  //port side (backwards)
  {PB11, 77, 1500, 1000, 1500, false, false},//mid-note, going up
  {PB10, 78, 1500, 1000, 1500, false, false},
  {PB1, 79, 1500, 1000, 1500, false, false},
  {PB0, 80, 1500, 1000, 1500, false, false},
  {PA7, 81, 1500, 1000, 1500, false, false},
  {PA6, 82, 1500, 1000, 1500, false, false},
  {PA5, 83, 1500, 1000, 1500, false, false},
  {PA4, 84, 1500, 1000, 1500, false, false},
  {PA3, 85, 1500, 1000, 1500, false, false},
  {PA2, 86, 1500, 1000, 1500, false, false},
  {PA1, 87, 1500, 1000, 1500, false, false},
  {PB2, 88, 1500, 1000, 1500, false, false},//borrowed PA0
  {PB2, 89, 1500, 1000, 1500, false, false},//ditto PC15
  {PC14, 91, 1500, 1000, 1500, false, false},
  {PC13, 93, 1500, 1000, 1500, false, false},//highest note
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
		SMT[i].activeTime = 0;//initialize this variable
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

	//static bool MillisecondTickHasReset{};//to save on math and only calulate this one time per cycle


	if (MillisecondTicks != LastMillisecondTicks && MillisecondTicks % 20 == 0)// !(isFinished & SERVO_HIT_REFRESH))
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
		//will not run the servo if it is not being used
		if(SMT[i].activeTime < SERVO_PUSH_TIME + (SERVO_PUSH_TIME / 2))//I've added a little extra time for the servo to make it back
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
		//digitalWrite(PC13, false);

		CONTINUOUS_SERVO.currentPPM = CONTINUOUS_SERVO.maxRange;
		ServoBackend(&CONTINUOUS_SERVO);
	}
	else
	{
		//digitalWrite(PC13, true);
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
		tower->activeTime = 0;
	}

	
	if(tower->minRange > tower->maxRange)
	{
		
		//everything here is an exact copy of the logic below, but inverted to account for the reversed inequalities
		if (tower->goForward == true)
		{
			if (tower->currentPPM > tower->maxRange)//tower is less than max range AND its max range is indeed larger than its min range
			{
				tower->currentPPM -= ((tower->minRange - tower->maxRange) * 2) / SERVO_PUSH_TIME;
			}
			else
			{
				tower->goForward = false;
			}
		}
		else if (tower->currentPPM < tower->minRange)
		{
			tower->currentPPM += ((tower->minRange - tower->maxRange) * 2) / SERVO_PUSH_TIME;
		}
		
	}
	else
	{
		
		if (tower->goForward == true)
		{
			//add a value to its angle
			if (tower->currentPPM < tower->maxRange)//tower is less than max range AND its max range is indeed larger than its min range
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

	if(tower->activeTime < SERVO_PUSH_TIME * 2)//this conditional prevents integer overflowing
		++tower->activeTime;//increment the timeout variable


	//original program (before the reversed support)
	/*
	if (tower->goForward == true)
	{
		//add a value to its angle
		if (tower->currentPPM < tower->maxRange)//tower is less than max range AND its max range is indeed larger than its min range
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
	*/


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
