#include <Wire.h>

#include "SparkFun_VL53L1X.h"
elapsedMillis msec = 0;        // used as a timer later
#include <ResponsiveAnalogRead.h>

SFEVL53L1X distanceSensor;

ResponsiveAnalogRead freqKnob(A0, true);
ResponsiveAnalogRead scaleKnob(A1, true);
int count = 0;                 //  variable to assign the different measured hand distances to...
int gateOld = LOW;
int gateNew = LOW;
int trig = LOW;
int count2 = 0;                 //variable to use in the actual sequencer
int scale[] = {0, 2, 3, 4, 5, 7, 7, 9, 10, 12, 12, 14, 16, 16, 17, 18, 19, 21, 22, 24, 24, 26, 27, 28, 29, 30, 31, 33, 34, 36, 36};             //  My weighted Mixolydianish Scale  Mixolydian + b3

int sequence[64]; // array to store the measured hand distances
// for example     sequence[count];

/*                                      //1    2  3   4   5   6   7
  int ionian[] =    {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35, 36};
  int dorian[] =    {0, 2, 3, 5, 7, 9, 10, 12, 14, 15, 17, 19, 21, 22, 24, 26, 27, 29, 31, 33, 34, 36};
  int harmo[] =    {0, 2, 3, 5, 7, 8, 11, 12,12, 14, 15, 17, 19, 20, 23,24, 24, 26, 25};

  int locrian[] =   {0, 1, 3, 5, 6, 8, 10, 12, 13, 15, 17, 19, 21, 22, 24, 25, 27, 29, 31, 33, 34, 36};
  int lydian[] =    {0, 2, 4, 6, 7, 9, 11, 12, 14, 16, 18, 19, 21, 23, 24, 26, 28, 30, 31, 32, 35, 36};
  int mixolydian[] ={0, 2, 4, 5, 7, 9, 10, 12, 14, 16, 17, 19, 21, 22, 24, 26, 28, 29, 31, 33, 34, 36};
  int aeolian[] =   {0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27, 29, 31, 32, 34, 36};
  int phrygian[] =  {0, 1, 3, 5, 7, 8, 10, 12, 13, 15, 17, 18, 20, 22, 24, 25, 27, 29, 30, 32, 34, 36};
*/

/*char *ion = ionian[];
  char *dor = dorian[];
  char *loc = locrian[];
  char *lyd = lydian[];
  char *mixo = mixolydian[];
  char *aeol = aeolian[];
  char *phr = phrygian[];
*/




//int mm = 0; //  Variable for Hand Distance  mm = millmeters

bool choice = 0;
bool gate = LOW;
void setup()
{

  if (distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");

  // Short mode max distance is limited to 1.3 m but has a better ambient immunity.
  // Above 1.3 meter error 4 is thrown (wrap around).
 // distanceSensor.setDistanceModeShort();
  distanceSensor.setDistanceModeLong(); // default


 
  distanceSensor.setTimingBudgetInMs(33);

  distanceSensor.setIntermeasurementPeriod(50);
  distanceSensor.startRanging(); // Start once
  analogWriteResolution(12); // sets DAC to 12bit ( 0 - 4095 )
  pinMode(5, INPUT_PULLUP);
  pinMode(7, INPUT);

  pinMode(17, OUTPUT);



}
int countSize = 15 ;
byte countFlag;

unsigned int mm;     ///////////////////
void loop()          ////////////////// LOOP BEGINS //
{



  freqKnob.update();
  scaleKnob.update();
  lll();

  gateNew = digitalRead(7);
  if (gateNew == HIGH && gateOld == LOW) {
    trig = HIGH;
  }
  else {
    trig = LOW;
  }
  gateOld = gateNew;

  if (trig) {
    if (digitalRead(5) == LOW)  {               //  If button is pressed
      recordSeq();




    }
    playSeq();







  }
}




//////////////////////////////////  END LOOP //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



void playSeq()  {                                       //  Play Sequence Function
  float dacUnit = 103.48 * scale[sequence[count2]];           //  converts  DAC units into Quantized Voltage
  int cv = round(dacUnit);


  analogWrite(A12, cv  );   /// write the dac with the correct pitch from the sequence
  count2++;
  if ( count2 > countSize)
  { count2 = 0;

  }


}

void recordSeq() {
  readsensor();              //Read Hand Position
  sequence[count] = mm;      // Record Hand position to Note array scale[];
  count++;
  if (count > countSize)
  { count = 0;

  }

}


void readsensor() {
  mm = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  // assign hand position to mm
  mm = map(mm, 47, 1290, 0, 32);

  if (mm > 31) {
    mm = 0;
  }


}
