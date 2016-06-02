#include <SharpIR.h>
#define tilt 8
#define ir A0
#define model 20150

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

// 25: the number of readings the library will make before calculating a mean distance
// 93: the difference between two consecutive measurements to be taken as valid
// model: an int that determines your sensor:  1080 for GP2Y0A21Y
//                                            20150 for GP2Y0A02Y
//                                            (working distance range according to the datasheets)
SharpIR sharp(ir, 10, 93, model);// ir: the pin where your sensor is attached
unsigned long time;
int pingDivisor = 0;
int tiltDivisor = 0;

void setup()
{
  pinMode(ir, INPUT);
  Serial.begin(9600);
}

void loop()
{
  bool output = false;
  time = millis();
  if ((time / 100) > pingDivisor)
  {
    ++pingDivisor;
    int dis = sharp.distance();
    Serial.print(dis);
    output = true;
  }
  if ((time / 1000) > tiltDivisor)
  {
    ++tiltDivisor;
    Serial.print(":");
    int upOrDown = digitalRead(tilt);
    if (upOrDown == HIGH)
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }
    output = true;
  }
  if (output == true)
  {
    Serial.println("");
  }
}
