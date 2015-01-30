#include <Servo.h> 

// A Joint is a servo mounted at a specific point on the robot arm.
class Joint
{
  private:
    int home;  // Initial position for the servo
    int step;  // Degrees by which to move servo on a single key press
    int min, max;   // Allow for physical boundaries; servo can move 0-180; but that's not necessarily true for a given joint
    Servo servo;  // Internal control object
    
  public:
    // TODO: Moar constructors?!?
    Joint(int pin, int interval, int start=90, int minimum=0, int maximum=180)
    {
      home = start;
      servo.attach(pin);
      goHome();
      step = interval;
      min = minimum;
      max = maximum;
    }
    
    void rotate(int amount)
    {
      servo.write(servo.read() + (amount * step));
      delay(50);
    }
    
    void goHome()
    {
      servo.write(home);
    }
};

Joint* shoulder;

void setup() 
{
  Serial.begin(115200);
  shoulder = new Joint(9, 2, 90);
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
    switch(Serial.read())
    {
      case 's':
        shoulder->rotate(smooth('s'));
        break;
      case 'S':
        shoulder->rotate(-1 * smooth('S'));
        break;
    }
  }
}

void loop() 
{ 
  getInput();
}
