#include <SoftPWM.h>
uint8_t leds[11] = {2, 3, 4, 5, 6, 7, 8, 9, 10};

 
void setup()
{
  // initialize serial communication at 9600 bits per second:
//  Serial.begin(9600);
  
  //start softPWNM
  SoftPWMBegin();

  for (int i = 0; i < 9; i++)
    SoftPWMSet(leds[i], 0);

  SoftPWMSetFadeTime(ALL, 50, 250);

  pwmMaxAdjPosition();
 
}

// uses an analog input to determine position of a POT, 
// Max range I want is 40% to 100%
// then converts the value into percentage to set the max PWM intensity

int pwmMaxAdjPosition(){
  //read the input on analog pin 0:
  double potData = analogRead(A0);
  
  //convert the read in data (ranges 0 to 1023 i.e. 2^10)
  double mAxAdjustPotPercent = (((potData/17)*100))+40;
   
      if (mAxAdjustPotPercent <40){
     
        mAxAdjustPotPercent = 40;
      }  
      else if (mAxAdjustPotPercent >100) {
        mAxAdjustPotPercent= 100;
      }
      return(mAxAdjustPotPercent);
}


// uses an analog input to determine position of a POT, 
// then converts the value into a number from 25 to 400ms

int scanDelay(){
  
  //read the input on analog pin 1:
  double potA1Data = analogRead(A1);
  //convert the read in data (ranges 0 to 1023 i.e. 2^10)
  double scanDelay = (potA1Data/2.73);
  
  if (scanDelay <25){
    scanDelay = 25;
  }
  return(scanDelay);
}



//Bounce Delay Function
// uses an analog input to determine position of a POT, 
// then converts the value into a number from 0 to 170ms
int bounceDelay(){
//try to base bounce Delay as a ratio of the scanDelay
  int bounceDelay =  (scanDelay()/5);
  return(bounceDelay);
}



void loop()
{

  int i;
 
  for (i =0; i < 8; i++)
  {
    SoftPWMSetPercent(leds[i+1], pwmMaxAdjPosition());
    SoftPWMSetPercent(leds[i], 0);
    delay(scanDelay());
  }
  
  delay(bounceDelay());
  
  for (i = 8; i > 0; i--)
  {
    SoftPWMSetPercent(leds[i-1], pwmMaxAdjPosition());
    SoftPWMSetPercent(leds[i], 0);
    delay(scanDelay());
  }

  delay(bounceDelay());


//Setup a string that will allow me to read out the settings




//print out the value you read:
 Serial.println("------New Data------------");
Serial.println("Scan Delay is currently set to: " + scanDelay());
Serial.println("Bounce delay is currently set to: " + bounceDelay());
Serial.println("PWM Max percentage is calulated and set to: " + pwmMaxAdjPosition());
Serial.println("------end----------------");
}
