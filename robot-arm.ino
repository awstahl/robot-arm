#include <Servo.h> 

// A Joint is a servo mounted at a specific point on the robot arm.
class Joint
{
  private:
    int pin;   // Arduino pin connected to servo control channel
    int home;  // Initial position for the servo
    int step;  // Degrees by which to move servo on a single key press
    int min, max;   // Allow for physical boundaries; servo can move 0-180; but that's not necessarily true for a given joint
    Servo servo;  // Internal control object
    
  public:
    Joint(int sPin, int interval=1, int start=90, int minimum=0, int maximum=180)
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
} *base, *shoulder, *elbow, *wrist, *yaw, *grip;

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
      amount = -1 * smooth(move);
    }
    else
    {
      amount = smooth(move);
    }
    joint->rotate(amount);
  }
}

void loop() 
{ 
  getInput();
}
