#include <Arduino.h>

unsigned long MillisecondTicks{};
unsigned long MicrosecondTicks{};

unsigned long divyMicroseconds{};
unsigned long previousMicrosecondTick{};

unsigned int servo1Angle{};
unsigned int servo1Delay{};

bool direct{};

void GetTicks(void)
{
  MillisecondTicks = millis();
  MicrosecondTicks = micros();

  
}


void setup() {
  // put your setup code here, to run once:
  //TEMP serves as our servo pin
  pinMode(4, OUTPUT);//servo 1
  //pinMode(10, OUTPUT);//servo 2
  //pinMode(13, OUTPUT);

  //serves as our button pin
  //pinMode(2, INPUT_PULLUP);

  servo1Angle = 800;
  
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  GetTicks();
  
  if(MillisecondTicks % 20 == 0)
  {
      previousMicrosecondTick = MicrosecondTicks;
      digitalWrite(4, 1);
     
  }
  else
  {
    divyMicroseconds = MicrosecondTicks - previousMicrosecondTick;

    if(servo1Angle < divyMicroseconds)
    {
      digitalWrite(4, 0);     
    }



    
  }



  if(MillisecondTicks % 600 == 0)
  {
    Serial.println(servo1Angle);
    if(++servo1Angle > 1200)
      servo1Angle = 300;

 
  }


}
