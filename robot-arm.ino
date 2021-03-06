/*

 No license, no warranty.  Use at your own risk.

*/

/*

 TODO: Create a 'robot appendage' object, which is a binding of Joint
 objects with physically-defined constraints.
 
 TODO: Add proximity sensors that allow the robot to detect the location
 of a nearby object and snap at it like a snake.
 
 TODO: Mount an infrared beam on the end of the grippers
 
 */

#include <Servo.h>
#include <ctype.h>

const byte BASE_PIN = 2;
const byte ELBOW_PIN = 4;
const byte GRIP_PIN = 7;
const byte SHOULDER_PIN = 3;
const byte WRIST_PIN = 5;
const byte YAW_PIN = 6;


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

  // Rotate the joint by <amount> degrees
  void rotate(byte amount)
  {
    write(servo.read() + amount);
  }
  
  // Move the joint to a specific location
  void locate(int location)
  {
    write(location);
  }
  
  // Get the current joint angle
  byte location()
  {
    return servo.read();
  }

  // Return the joint to its home angle
  void goHome()
  {
    write(home);
  }
};


// TODO: Serialize & read in at runtime in a manager
Joint* base;
Joint* elbow;
Joint* grip;
Joint* shoulder;
Joint* wrist;
Joint* yaw;


/*
  Robot modeling
  - Collection of one or more appendages,sensors, and movement interfaces
  - Provides methods for pre-defined sequences
  - Use an inverse kinematic subsystem
*/
/*class Robot
{
private:
  
public:

  void move(char key)
  {
    
  }
};*/


// TODO: Refactor into Robot
class JointManager
{

private:

  // Tally the number of sequential character presses to avoid stuttering
  static byte smooth(char c)
  {
    byte i = 1;
    while(Serial.peek() == c)
    {
      Serial.read();
      i++;
    }
    return i;
  }

  // Ugh... how not to write a mile of c++ to avoid this sort of coupling...
  static Joint* selectJoint(char key)
  {
    Joint* joint;
    byte step;       // TODO: Degrees by which to move servo on a single key press; requires testing
    switch(tolower(key))
    {
    case 'b':
      joint = base;
      break;
    case 'e':
      joint = elbow;
      break;
    case 'g':
      joint = grip;
      break;
    case 's':
      joint = shoulder;
      break;
    case 'w':
      joint = wrist;
      break;
    case 'y':
      joint = yaw;
      break;
    default:
      joint = 0;
    }
    return joint;
  }

public:

  // Based on one or more key presses of a single character, move the joint
  static void move(char key)
  {
    Joint* joint = selectJoint(key);
    int amount = 0;
    int direction = 1;

    if ( isupper(key) ) 
    {
      direction = -1;
    }
    amount = direction * smooth(key);

    if (joint)
    {
      joint->rotate(amount);
    }
  }
};

void setup()
{
  Serial.begin(115200);

  // Joint(byte pin, byte start=90, byte minimum=0, byte maximum=180)
  base = new Joint(BASE_PIN);
  elbow = new Joint(ELBOW_PIN, 100, 45, 135);
  grip = new Joint(GRIP_PIN, 45);
  shoulder = new Joint(SHOULDER_PIN, 105, 45, 135);
  wrist = new Joint(WRIST_PIN);
  yaw = new Joint(YAW_PIN);
}


void loop() 
{
  if (Serial.available())
  {
    JointManager::move(Serial.read());
  }
}

