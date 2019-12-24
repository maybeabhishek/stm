#include <Arduino.h>


const int Kp = 0;
const int Kd = 0;
const int Ki = 0;
const int setPoint = 35;    
const int baseSpeed = 100;    
const int maxSpeed = 200;   
const int queueSize = 20; // Size of error queue - to store previous error values

const int rightPWM = PA0; 
const int rightDir = PA1; 
const int leftDir = PA2; 
const int leftPWM = PA3;  

int rightMotorSpeed;
int leftMotorSpeed;

int errorQueue[queueSize]; // error queue
int head = 0;  // points to the head of the queue
int curr = 0;  //points to the current index of the queue where the error is to be added


//initializes the errorQueue
for(int i = 0; i<queueSize;i++)
errorQueue[i]=0;


//Adds error value n to the errorQueue
void enqueue(int n){
  if(curr>queueSize)
  curr = 0;
  errorQueue[curr]=n;
  curr++;
}

//calucaltes sum of last 10 error values [used for Ki]
int errorSum(){
  int sum = 0;
  if((curr-head)<=10){
    for(int i =head; i<=curr; i++)
      sum+=errorQueue[i];
    
    return sum;
  }
  else if(curr==head){
    for (int i = queueSize - 10; i < queueSize; i++)
      sum+=errorQueue[i];
    sum+=errorQueue[curr];

    return sum;
  }
  else{
    for (int i = curr-10; i <= curr; i++)
      sum+=errorQueue[i];
    
    return sum;
  }
}

void setup() {
  
  
  pinMode(rightPWM, PWM);
  pinMode(leftPWM, PWM);
  pinMode(rightDir,OUTPUT);
  pinMode(leftDir,OUTPUT);
  pwmWrite(rightDir,HIGH);
  pwmWrite(leftDir,HIGH);

  Serial1.begin(9600);
}

int lastError = 0;    
void loop() {
  while(Serial1.available() <= 0); 
  int positionVal = Serial1.read();    
  if(positionVal == 255) {
    pwmWrite(pwm1,0);
    pwmWrite(pwm2,0);
  }

  else {
    int error = positionVal - setPoint;   
    enqueue(error);   //appends error to the queue
    int motorSpeed = Kp * error + Kd * (error - lastError) + Ki*errorSum();   //Added Ki term to the equation [helpful for curves]
    lastError = error;   
    

    // Changed the way we append values to the motors
    if(error>=0){
      rightMotorSpeed = baseSpeed;
      leftMotorSpeed = baseSpeed + motorSpeed;
    }
    else{
      rightMotorSpeed = baseSpeed - motorSpeed;
      leftMotorSpeed = baseSpeed;
    }
    //till here
    
    if(rightMotorSpeed > maxSpeed) rightMotorSpeed = maxSpeed;
    if(leftMotorSpeed > maxSpeed) leftMotorSpeed = maxSpeed;

    
    if(rightMotorSpeed < 0) rightMotorSpeed = 0;
    if(leftMotorSpeed < 0) leftMotorSpeed = 0;

    
    pwmWrite(pwm1,rightMotorSpeed);
    pwmWrite(pwm2,leftMotorSpeed);
  }

}