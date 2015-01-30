#include <Servo.h> 

// Need to use globals to be available to both setup() and loop()
Servo shoulder;
Servo elbow;

// A Joint is a servo mounted at a specific point on the robot arm.
class Joint
{
  private:
    int home;  // Initial position for the servo
    int step;  // Degrees by which to move servo on a single key press
    int max, min;   // Allow for physical boundaries; servo can move 0-180; but that's not necessarily true for a given joint
    Servo servo;  // Internal control object
    
  public:
    // TODO: Provide reasonable constructors...    
    Joint(int pin=-1, int start=90)
    {
      home = start;
//      servo.attach(pin);
    }
};

void setup() 
{
  Serial.begin(115200);
  Joint joint = Joint(17, 89);
  shoulder.attach(9);
  shoulder.write(90);
  elbow.attach(7);
  elbow.write(90);
}

void rotate(Servo servo, int amt)
{
  servo.write(servo.read() + amt);
  delay(25);
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
  int degrees = 2;
  int cur;
  if (Serial.available())
  {
    switch(Serial.read())
    {
      case 's':
        rotate(shoulder, degrees * smooth('s'));
        break;
      case 'S':
        rotate(shoulder, -degrees * smooth('S'));
        break;
    }
  }
}

void loop() 
{ 
  getInput();
}
