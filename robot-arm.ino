/*

TODO: Add proximity sensors that allow the robot to detect the locationn
of a nearby object and snap at it like a snake.

TODO: Create a 'robot appendage' object, which is a binding of Joint
objects with physically-defined constraints.

TODO: Incorporate various other sensors to improve robot conntrol.
(i.e. gyroscopes)

*/

#include <Servo.h> 

// A Joint is a servo mounted at a specific point on a robot.
class Joint
{
  private:
    int pin;   // Arduino pin connected to servo control channel
    int home;  // Initial position for the servo
    int step;  // Degrees by which to move servo on a single key press
    int min, max;   // Allow for physical boundaries; servo can move 0-180; but that's not necessarily true for a given joint
    Servo servo;  // Internal control object
    
  public:
    Joint(int sPin, int interval=1, int start=90, int minimum=0, int maximum=180, int delay=0)
    {
      pin = sPin;
      home = start;
      servo.attach(pin);
      goHome();
      step = interval;
      min = minimum;
      max = maximum;
    }

    void rotate(int amount)
    {
      int dest = servo.read() + (step * amount);
      if ((dest <= max) && (dest >= min))
      {
        servo.write(dest);
      }
      else if (dest < min)
      {
        servo.write(min);
      }
      else // (dest > max)
      {
        servo.write(max);
      }
    }
    
    void goHome()
    {
      servo.write(home);
    }
};

/*
  Appdendage modeling...
  - An appendage is a set of two or more joints, which may or may not have a physical relation.
  - Movement of the appendage may require coordination of two or more joints
  - An appendage can provide custom movement interfaces.
  - An appendage must be able to move to a point in space.
*/
class Appendage
{
  private:
    Joint* joints[];
    
  public:
    Appendage(int fake)
    {
      fake * 1;
    }
};


void setup() 
{
  Serial.begin(115200);
  
  // TODO: Check pin-outs once circuit is actually built
  base = new Joint(9, 5);
  elbow = new Joint(8, 2, 150, 45, 135);
  grip = new Joint(7, 10, 45);
  shoulder = new Joint(6, 5, 105, 75, 135);
  wrist = new Joint(5, 3);
  yaw = new Joint(4, 3);
}

int smooth(char c)
{
  int i = 1;
  while(Serial.peek() == c)
  {
    Serial.read();
    i++;
  }
  return i;
}

void getInput()
{
  if (Serial.available())
  {
    Joint* joint;
    int amount = 0;
    int direction = 1;
    char move = Serial.read();
    
    switch(move)
    {
      case 'b':
      case 'B':
        joint = base;
        break;
      case 'e':
      case 'E':
        joint = elbow;
        break;
      case 'g':
      case 'G':
        joint = grip;
        break;
      case 's':
      case 'S':
        joint = shoulder;
        break;
      case 'w':
      case 'W':
        joint = wrist;
        break;
      case 'y':
      case 'Y':
        joint = yaw;
        break;
    }
    
    if ((move >= 'A') && (move <= 'Z'))
    {
      direction = -1;
    }
    // TODO: Test out the short cirtuit directive...
    // (move >= 'A') && (move <= 'Z') && (direction = -1)
    amount = direction * smooth(move);
    joint->rotate(amount);
  }
}

void loop() 
{ 
  getInput();
}
