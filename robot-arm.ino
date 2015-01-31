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
    // TODO: Moar constructors?!?  Validation logic?  (Don't use idiotically... it's not a lib yet)
    Joint(int sPin, int interval, int start=90, int minimum=0, int maximum=180)
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
} *shoulder;

void setup() 
{
  Serial.begin(115200);
  shoulder = new Joint(9, 5, 105, 75, 135);
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
    // TODO: Use the char as a lookup index
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
