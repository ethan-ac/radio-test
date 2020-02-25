
//(pi*radius[cm])/(600*velocity[cm/s])
//5V to blue
//GND to green

//--------------------
//TODO:
//Add 180 deg turn function
//Improve readability and variable/function names
//--------------------

//pin setup for motors
#define MOTOR_A    1
#define MOTOR_A_CCW 8
#define MOTOR_A_CW  9
#define PWM_A   10

#define MOTOR_B 0
#define MOTOR_B_CCW 6
#define MOTOR_B_CW  4
#define PWM_B   5

//motor speed 0-1023
int motorValueA = 0;
int motorValueB = 0;

//desired speed in cm/s
float givenSpeedA = 1;
float givenSpeedB = 0;

//desired time interval for encoders
unsigned long givenTimeA = 0;
unsigned long givenTimeB = 0;

//desired direction
bool dirA = false;
bool dirB = true;

//check for if interrupt has been triggered recently
bool encoderStateA = false;
bool encoderStateB = false;

//current/previous encoder time intervals
unsigned long timeAcur = 0;
unsigned long timeBcur = 0;
unsigned long timeApre = 0;
unsigned long timeBpre = 0;
//actual time interval for encoders
unsigned long actualTimeA = 0;
unsigned long actualTimeB = 0;

void setup()
{
  pinMode(MOTOR_A_CCW, OUTPUT);
  pinMode(MOTOR_A_CW, OUTPUT);
  pinMode(PWM_A, OUTPUT);

  pinMode(MOTOR_B_CCW, OUTPUT);
  pinMode(MOTOR_B_CW, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  pinMode(2, INPUT);
  pinMode(3, INPUT);

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(3), EncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(2), EncoderB, RISING);
}

void loop()
{
  //interruptCheck()
  //calculates desired encoder time interval based on desired speed in cm/s
  //speedCalc();
  //Serial.println(actualTimeA);
  //sets motor speed and directions
  //motorA(dirA, motorValueA);
  //motorB(dirB, motorValueB);
  turn();
  delay(1000);
}

//--------------------------

void turn()
{
  noInterrupts();
  for (int i = 0; i < 100; i++) {
    digitalWrite(MOTOR_A_CCW, true);
    digitalWrite(MOTOR_A_CW, false);
    analogWrite(PWM_A, 255);
    digitalWrite(MOTOR_B_CCW, false);
    digitalWrite(MOTOR_B_CW, true);
    analogWrite(PWM_B, 255);
    delay(1);
  }
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
  interrupts();
}

//calls micros() when interrupt is triggered, keeps previous micros() value for calculating time interval of encoder
void EncoderA()
{
  timeApre = timeAcur;
  timeAcur = micros();
  encoderStateA = true;
}
void EncoderB()
{
  timeBpre = timeBcur;
  timeBcur = micros();
  encoderStateB = true;
}

//sets motor speed and direction, direction is single bool, motor speed is scaled from digital input to analog output (0-1023 -> 0-255)
void motorA(bool dirA, int motorValueA)
{
  digitalWrite(MOTOR_A_CCW, dirA);
  digitalWrite(MOTOR_A_CW, !dirA);
  motorValueA = map(motorValueA, 0, 1023, 0, 255);
  analogWrite(PWM_A, motorValueA);
}
void motorB(bool dirB, int motorValueB)
{
  digitalWrite(MOTOR_B_CCW, dirB);
  digitalWrite(MOTOR_B_CW, !dirB);
  motorValueB = map(motorValueB, 0, 1023, 0, 255);
  analogWrite(PWM_B, motorValueB);
}

//takes desired speed in cm/s and converts to encoder time interval
void speedCalc(float givenSpeedA, float givenSpeedB)
{
  givenTimeA = 23.562 / (givenSpeedA / 60);
  givenTimeB = 23.562 / (givenSpeedB / 60);
  //if the desired encoder time interval is more than the actual encoder time interval increases motor speed by 10, and vice versa
  if (givenTimeA > actualTimeA)
  {
    motorValueA += 1;
  }
  else if (givenTimeA < actualTimeA)
  {
    if (motorValueA != 0)
    {
      motorValueA -= 1;
    }
  }
  if (givenTimeB > actualTimeB)
  {
    motorValueB += 1;
  }

  else if (givenTimeB < actualTimeB)
  {
    if (motorValueB != 0)
    {
      motorValueB -= 1;
    }
  }
}

void brake(int length)
{
  noInterrupts();
  digitalWrite(MOTOR_A_CCW, HIGH);
  digitalWrite(MOTOR_A_CW, HIGH);
  digitalWrite(PWM_A, HIGH);

  digitalWrite(MOTOR_B_CCW, HIGH);
  digitalWrite(MOTOR_B_CW, HIGH);
  digitalWrite(PWM_B, HIGH);
  delay(length);

  interrupts();
}

void interruptCheck()
{
  //checks if interrupt has been triggered
  if (encoderStateA)
  {
    actualTimeA = timeAcur - timeApre;
    encoderStateA = false;
  }
  if (encoderStateB)
  {
    actualTimeB = timeBcur - timeBpre;
    encoderStateB = false;
  }
}
