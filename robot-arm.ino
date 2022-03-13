/*

Robot Arm Code
MIT License

© 2022 Alexander W. Stahl

*/

#include <Servo.h>
#include <SharpIR.h>
#include <ctype.h>


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

// A sensor is a, well, sensor to provide input to the robot
// TODO: historical values & current readings in same class?
class RangeSensor
{
  private:
  SharpIR* sensor;
  int minimum, maximum, count, *values;
  long sum;

  public:

  RangeSensor(byte pin, int type)
  {
    sensor = new SharpIR(pin, type);
    minimum = maximum = sum = count = 0;
  }

  uint8_t read()
  {
    uint8_t distance = sensor->distance();
//    values[count] = distance;
    count++;
    sum += distance;

    if (distance > maximum)
    {
      maximum = distance;
    }
    if (distance < minimum)
    {
      minimum = distance;
    }
    return distance;
  }

  byte getAverage()
  {
    return (sum / count);
  }

  byte getCount()
  {
    return count;
  }

  long getSum()
  {
    return sum;
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


/*
 * The Robot is essentially a collection of subsystems to:
 * - receive input from sensors 
 * - evaluate input
 * - react in the form of movement
 * - react with personality
 */
class Robot
{
  private:
  // TODO
    
  public:

  Robot()
  {
    // TODO
  }

  void act()
  {
    // TODO
  }
};


void setup()
{
  Serial.begin(115200);

  base = new Joint(BASE_PIN);
  elbow = new Joint(ELBOW_PIN, 100, 45, 135);
  grip = new Joint(GRIP_PIN, 45);
  shoulder = new Joint(SHOULDER_PIN, 105, 45, 135);
  wrist = new Joint(WRIST_PIN);
  yaw = new Joint(YAW_PIN);
  lrange = new RangeSensor(RANGE_L, 1080);
//  srange = new RangeSensor(RANGE_S, 430);
}


int reading;
void loop() 
{
  Serial.print("Long range reading: ");
  reading = lrange->read();
  Serial.println(reading);
  if (reading <= 81)
  {
    if (reading < 20)
    {
      snake();
      delay(250);
    }
    else if (reading < 40)
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
  base->reset();
  shoulder->reset();
  elbow->reset();
  wrist->reset();
  yaw->reset();
  grip->reset();
  delay(250);
  Serial.print("Long range samples: ");
  Serial.println(lrange->getCount());
  Serial.print("Long range sum: ");
  Serial.println(lrange->getSum());
  Serial.print("Long range average: ");
  Serial.println(lrange->getAverage());
  delay(2500);
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
