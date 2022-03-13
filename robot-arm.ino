/*

Robot Arm Code
MIT License

© 2022 Alexander W. Stahl

*/

#include <Servo.h>
#include <SharpIR.h>
#include <ctype.h>
#include <ArduinoLog.h>


// Servo pins
const byte BASE_PIN = 6;
const byte ELBOW_PIN = 8;
const byte GRIP_PIN = 5;
const byte SHOULDER_PIN = 7;
const byte WRIST_PIN = 4;
const byte YAW_PIN = 3;

// Sensor pins
const byte RANGE_L = A0;
const byte RANGE_S = A1;

class Sensor
{
  protected:
  int minimum, maximum, count, *values;
  long sum;

  void store()
  {
    values[count] = value;
    count++;
    sum += value;

    if (value > maximum)
    {
      maximum = value;
    }
    if (value < minimum)
    {
      minimum = value;
    }
  }


  public:
  uint8_t value;

  int getAverage()
  {
    return (sum / count);
  }

  int getCount()
  {
    return count;
  }

  long getSum()
  {
    return sum;
  }
};


// A range sensor is a device to measure distance to an object
class RangeSensor : public Sensor
{
  private:
  SharpIR* sensor;
  byte pin;

  public:

  RangeSensor(byte pin, int type)
  {
    pin = pin;
    sensor = new SharpIR(pin, type);
    minimum = maximum = sum = count = 0;
  }

  uint8_t read()
  {
    value = sensor->distance();
    Log.trace("Range Sensor on pin %d reads at: %d" NL, pin, value);
    store();
    return value;
  }
};


// A Joint is a servo mounted at a specific point on a robot.
class Joint
{
  private:
  byte home;       // Initial position for the servo
  byte min, max;   // Allow for physical boundaries; servo can move 0-180; but that's not necessarily true for a given joint
  Servo servo;     // Internal control object
  
  // Write a valid value to the arduino pin for the local servo
  void write(byte dest)
  {
    byte val;
    if (dest <= min)
    {
      val = min;
    }
    else if (dest >= max)
    {
      val = max;
    }
    else  // valid destination
    {
      val = dest;
    }
    servo.write(val);
  }

  public:

  Joint(byte pin, byte start=90, byte minimum=0, byte maximum=180)
  {
    home = start;
    servo.attach(pin);
    min = minimum;
    max = maximum;
    reset();
  }

  void rotate(int amount)
  {
    write(servo.read() + amount);
  }
  
  void locate(byte location)
  {
    write(location);
  }
  
  byte location()
  {
    return servo.read();
  }

  void reset()
  {
    write(home);
  }
};


// TODO: Can these be serialized & read in at runtime in a manager?
Joint* base;
Joint* elbow;
Joint* grip;
Joint* shoulder;
Joint* wrist;
Joint* yaw;

// TODO: How to manage the range sensors?
RangeSensor* lrange;
RangeSensor* srange;


void setup()
{
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.verbose("Started Logger; proceeding with setup()" NL);

  base = new Joint(BASE_PIN);
  elbow = new Joint(ELBOW_PIN, 100, 45, 135);
  grip = new Joint(GRIP_PIN, 45);
  shoulder = new Joint(SHOULDER_PIN, 105, 45, 135);
  wrist = new Joint(WRIST_PIN);
  yaw = new Joint(YAW_PIN);
  lrange = new RangeSensor(RANGE_L, 1080);
  srange = new RangeSensor(RANGE_S, 430);
}


int l_reading;
int s_reading;
void loop() 
{
  l_reading = lrange->read();
  s_reading = srange->read();
  Log.trace("Long range reading: %d" NL, l_reading);
  Log.trace("Short range reading: %d" NL, s_reading);

  if (l_reading <= 81)
  {
    if (l_reading < 20)
    {
      snake();
      delay(250);
    }
    else if (l_reading < 40)
    {
      headbang();
      delay(250);
    }
    else
    {
      shake();
      delay(250);
    }
  }
  else
  {
    tweak();
  }
  reset();
  delay(250);
  Log.trace("Long range samples: %d" NL, lrange->getCount());
  Log.trace("Long range sum: %d" NL, lrange->getSum());
  Log.trace("Long range average: %d" NL, lrange->getAverage());
  delay(2500);
}


/* TODO: These funcs are the major pain points...
 *  - series of looped sequential movements
 *  - how to model this behavior programmatically?
*/

void reset()
{
  base->reset();
  shoulder->reset();
  elbow->reset();
  wrist->reset();
  yaw->reset();
  grip->reset();
}

void headbang()
{
  for (int j=0; j<10; j++)
  {
    for (int i=0; i<5; i++)
    {
      shoulder->rotate(5);
      wrist->rotate(30);
      delay(50);
    }
    
    for (int i=0; i<5; i++)
    {
      shoulder->rotate(-5);
      wrist->rotate(-30);
      delay(50);
    }
  }
}


void shake()
{
  for (int i=0; i<5; i++)
  {
    base->rotate(45);
    yaw->rotate(60);
    grip->rotate(45);
    delay(200);
    base->rotate(-45);
    yaw->rotate(-60);
    grip->rotate(-45);
    delay(200);
  }
}


void snake()
{
  shoulder->locate(100);
  elbow->locate(45);
  wrist->locate(10);
  delay(1000);

  for (int i=0; i<5; i++)
  {
    base->rotate(60);
    delay(500);
    base->rotate(-60);
    delay(500);
  }
}


void tweak()
{
  yaw->rotate(25);
  grip->rotate(45);
  delay(500);
  yaw->rotate(-25);
  grip->rotate(-45);
  delay(1000);
}
