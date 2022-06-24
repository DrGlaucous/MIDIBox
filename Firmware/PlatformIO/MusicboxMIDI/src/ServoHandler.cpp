#include <Arduino.h>
#include "configuration.h"
#include "ServoHandler.h"
//#include <Control_Surface.h>


//Servo Map Table
SERVO_ID SMT[] =
{
	//pin, note, minRange, maxRange, currentPPM, requestNote, goForward, dwellTime, 
	//main side (forwards)
	{PB12, 53, 900, 1500, 900, false, false, 0},//0**lowest note, going up
	{PB13, 55, 950, 1500, 950, false, false, 0},//1**
	{PB14, 60, 1040, 1700, 1040, false, false, 20},//2**
	{PB15, 62, 950, 1650, 950, false, false, 0},//3
	{PA8, 64, 1000, 1530, 1000, false, false, 30},//4**
	{PA11, 65, 1000, 1550, 1000, false, false, 0},//5USB- (unplug these two for USB serial)
	{PA12, 67, 970, 1600, 970, false, false, 0},//6USB+
	{PA15, 69, 950, 1600, 950, false, false, 0},//7
	{PB3, 70, 1050, 1600, 1050, false, false, 0},//8
	{PB4, 71, 950, 1550, 950, false, false, 0},//9
	{PB5, 72, 1000, 1600, 1000, false, false, 0},//10**C
	{PB6, 73, 950, 1500, 950, false, false, 0},//11**C#
	{PB7, 74, 950, 1550, 950, false, false, 0},//12**D
	{PB8, 75, 1000, 1600, 1000, false, false, 0},//13**
	{PB9, 76, 1000, 1600, 1000, false, false, 0},//14**
	//port side (backwards)
	{PB11, 77, 1500, 900, 1500, false, false, 0},//15**F mid-note, going up
	{PB10, 78, 1600, 970, 1600, false, false, 0},//16**
	{PB1, 79, 1470, 900, 1470, false, false, 40},//17**G
	{PB0, 80, 1600, 1000, 1600, false, false, 0},//18**
	{PA7, 81, 1530, 1000, 1530, false, false, 30},//19**A
	{PA6, 82, 1600, 1000, 1600, false, false, 0},//20**
	{PA5, 83, 1500, 950, 1500, false, false, 0},//21**B
	{PA4, 84, 1420, 900, 1420, false, false, 40},//22**C
	{PA3, 85, 1600, 1000, 1600, false, false, 0},//23**
	{PA2, 86, 1500, 900, 1500, false, false, 0},//24**D
	{PA1, 87, 1420, 900, 1420, false, false, 50},//25**
	{PA0, 88, 1530, 970, 1530, false, false, 20},//26**E borrowed
	{PC15, 89, 1600, 1050, 1600, false, false, 0},//27**ditto
	{PC14, 91, 1580, 980, 1580, false, false, 0},//28**
	{PC13, 93, 1500, 950, 1500, false, false, 0},//29**highest note//PC13 (when debugging, I use placeholder PB2 so I can connect the stlink (the wafer servo uses this pin))
};



SERVO_ID CONTINUOUS_SERVO =
	//pin, note, minRange, maxRange, currentPPM, requestNote, goForward
	{CONTINUOUS_SERVO_PIN, 0, CONTINUOUS_REST_ANGLE, CONTINUOUS_ACTIVE_ANGLE, CONTINUOUS_REST_ANGLE, false, false};


//these should be with the GetTicks() function, but in order to use them in other pages, I would need to make another header file, and I'm lazy
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
		//else if(SMT[i].pinNo == PA7)//for debugging
		//	ServoBackend(&SMT[i]);

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

	//this will only attach the servo if it needs to be run, which will save CPU cycles
	if(waferTimeout < CONTINUOUS_RUN_TIME)
	{
		//digitalWrite(PC13, false);

		CONTINUOUS_SERVO.currentPPM = CONTINUOUS_SERVO.maxRange;
		ServoBackend(&CONTINUOUS_SERVO);
	}
	//this conditional was only needed for debugging
	/*else
	{
		//digitalWrite(PC13, true);
		CONTINUOUS_SERVO.currentPPM = CONTINUOUS_SERVO.minRange;
		ServoBackend(&CONTINUOUS_SERVO);
	}*/


}

//sweep each servo when a trigger variable is called
void SingleServoSweep(SERVO_ID *tower)
{


	if (tower->requestNote == true)
	{
		tower->requestNote = false;
#ifdef WAIT_UNTIL_DONE
		if(tower->activeTime >= SERVO_PUSH_TIME)//will only push again if the tower has been active for at least the required ammount
		{
			tower->goForward = true;
			tower->activeTime = 0;
		}
#else
		tower->goForward = true;
		tower->activeTime = 0;
#endif

	}





#ifdef SMOOTH_SWEEP
	if(tower->minRange > tower->maxRange)
	{
		
		//everything here is an exact copy of the logic below, but inverted to account for the reversed inequalities
		if (tower->goForward == true)
		{
			if (tower->currentPPM > tower->maxRange)//tower is less than max range AND 
			{
				tower->currentPPM -= (tower->minRange - tower->maxRange) / ((SERVO_PUSH_TIME / 2) - tower->dwellTime);
			}
			else if(SERVO_PUSH_TIME < tower->activeTime * 2)
			{
				tower->goForward = false;
			}
		}
		else if (tower->currentPPM < tower->minRange)
		{
			tower->currentPPM += (tower->minRange - tower->maxRange) / ((SERVO_PUSH_TIME / 2) - tower->dwellTime);
		}
		
	}
	else
	{
		
		if (tower->goForward == true)
		{
			//add a value to its angle
			if (tower->currentPPM < tower->maxRange)//tower is less than max range
			{
				//ratio of PPM range to time minus the reserved ammount(the /2 is the result of the need to devide this between forward and backward motion)
				//the function assumes it will be called every Millisecond
				tower->currentPPM += (tower->maxRange - tower->minRange) / ((SERVO_PUSH_TIME / 2) - tower->dwellTime);

			}
			else if(SERVO_PUSH_TIME < tower->activeTime * 2)//the servo has been active for 1/2 of the total allowed time
			{
				tower->goForward = false;
			}


		}
		else if (tower->currentPPM > tower->minRange)
		{
			tower->currentPPM -= (tower->maxRange - tower->minRange) / ((SERVO_PUSH_TIME / 2) - tower->dwellTime);
		}
		

	}
#else

	if(tower->activeTime * 2 < SERVO_PUSH_TIME && tower->goForward == true)//it has been pushing for 1/2 of the total allowed push time
		tower->currentPPM = tower->maxRange;
	else
	{
		tower->currentPPM = tower->minRange;
		tower->goForward = false;
	}

#endif





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
