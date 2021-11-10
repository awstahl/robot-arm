/*

Robot Arm Code
MIT License

© 2021 Alexander W. Stahl

*/


#include <Servo.h>
#include <ctype.h>


// Servo pins
const byte BASE_PIN = 6;
const byte ELBOW_PIN = 8;
const byte GRIP_PIN = 5;
const byte SHOULDER_PIN = 7;
const byte WRIST_PIN = 4;
const byte YAW_PIN = 3;

// Sensor pins
const byte MIC_A_PIN = A5;
const byte MIC_D_PIN = 9;


// A sensor is a, well, sensor to provide input to the robot
class Sensor
{
  private:
  byte pin;
  bool digital;

  public:

  Sensor(byte sensorPin, bool digi=1)
  {
    pin = sensorPin;
    digital = digi;
  }

  int read()
  {
    if (digital)
    {
      return digitalRead(pin);
    }
    else
    {
      return analogRead(pin);
    }
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
  void write(int dest)
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
    goHome();
  }

  void rotate(int amount)
  {
    write(servo.read() + amount);
  }
  
  void locate(int location)
  {
    write(location);
  }
  
  byte location()
  {
    return servo.read();
  }

  void goHome()
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


class Kinematics
{
  private:

  public:

  Kinematics()
  {
    
  }
};

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
  // TODO: should consist of a set of subsystems which don't necessarily exist yet
    
  public:

  Robot()
  {
    // TODO: implement
  }


  // Should accept a struct of sensor values (when that struct exists...)
  void act()
  {
    // TODO: implement
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

  base->goHome();
  delay(250);

  shoulder->goHome();
  delay(250);

  elbow->goHome();
  delay(250);

  wrist->goHome();
  delay(250);

  yaw->goHome();
  delay(250);

  grip->goHome();
  delay(250);
}


void loop() 
{
//  smoke();
  wrist->rotate(60);
  delay(500);
  grip->rotate(45);
  delay(500);
  wrist->rotate(-60);
  delay(500);
  grip->rotate(-45);
  delay(500);
}


void smoke()
{
  Serial.println("Moving base");
  base->rotate(10);
  delay(500);

  Serial.println("Moving shoulder");
  shoulder->rotate(10);
  delay(500);

  Serial.println("Moving elbow");
  elbow->rotate(5);
  delay(500);

  Serial.println("Moving wrist");
  wrist->rotate(10);
  delay(500);

  Serial.println("Moving yaw");
  yaw->rotate(10);
  delay(500);

  Serial.println("Moving grip");
  grip->rotate(5);
  delay(2500);

  // Reverse back to start:
  Serial.println("Moving base back");
  base->rotate(-10);
  delay(500);

  Serial.println("Moving shoulder back");
  shoulder->rotate(-20);
  delay(500);

  Serial.println("Moving elbow back");
  elbow->rotate(-25);
  delay(500);

  Serial.println("Moving wrist back");
  wrist->rotate(-20);
  delay(500);

  Serial.println("Moving yaw back");
  yaw->rotate(-20);
  delay(500);

  Serial.println("Moving grip back");
  grip->rotate(-5);
  delay(2500);
}
