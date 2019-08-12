#include <Wire.h>

// Pin assignments
const int motor_dr = 33; //motor_dr  pin > Phase: determines Motor Direction
const int motor_pwm = 32; //motor_pwm pin > Enable pin of the DRV8835 
const int steer_motor_dr = 26; //motor_dr  pin > Phase: determines Motor Direction
const int steer_motor_pwm = 25; //motor_pwm pin > Enable pin of the DRV8835 
const int white_led = 2; //forward direction led
const int red_led = 4; //reverse direction led
const int photocell_pin = 36; //photocell analog input
const int trig_pin = 16; //ultrasonic sensor trigger signal
const int echo_pin = 17; //ultrasonic sensor receive signal
const int start_pin = 35; //start motion

// internal addresses
char accel_speed = 'H';
int distance_actual = 0;
int fwd_seq = 1;
int rev_seq = 1;
long duration, inches = 0;
String moveDir = "stpCmd";
bool prgEnbl;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;
int fwd_accel_speed = 180;
int fwd_run_speed = 80;
int rev_accel_speed = 200;
int rev_run_speed = 128;
int stop_speed = 0;

// Setting stear PWM properties
const int freqSteer = 30000;
const int pwmChannelSteer = 1;
const int resolutionSteer = 8;
int dutyCycleSteer = 256;

void setup() {

  // start serial monitor
  Serial.begin(115200);
  
  // sets the discrete pins:
  pinMode(motor_dr, OUTPUT);
  pinMode(motor_pwm, OUTPUT);
  pinMode(steer_motor_dr, OUTPUT);
  pinMode(steer_motor_pwm, OUTPUT);
  pinMode(white_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(start_pin, INPUT);
  
  // configure LED PWM  functionalitites
  ledcSetup(pwmChannel, freq, resolution);// This is a controller function to configure the PWM for fwd/rev
  ledcSetup(pwmChannelSteer, freqSteer, resolutionSteer);// This is a controller function to configure the PWM for left/right
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(motor_pwm, pwmChannel);
  ledcAttachPin(steer_motor_pwm, pwmChannelSteer);

  // begin i2c communication for lcd
  Wire.begin(21,22); //asign SDA to pin 21 and SCL to pin 22

}

void loop() {

    prgEnbl = digitalRead(start_pin);
    lightControl();
    if(prgEnbl == HIGH){
      Serial.print("Start Initiated\n");
      if (distanceSense() > 20){
      rev_seq = 1;
      moveDir = "fwdCmd";
        switch (fwd_seq){
          case 1:
            moveVehicle(moveDir);
            fwd_seq = 2;
          case 2:
            moveStraight();
            break;
          default:
            stopVehicle();
            break;
          }
      }
      if (distanceSense() < 20){
        decelVehicle();
        stopVehicle();
        fwd_seq = 1;
        moveDir = "revCmd";
          switch (rev_seq){
            case 1:
              moveRight();
              rev_seq = 2;
            case 2:
              moveVehicle(moveDir);
              rev_seq = 3;
            case 3:
              while (distanceSense() < 12){
                ledcWrite(pwmChannel, dutyCycle);
              }
              delay(500);
              rev_seq = 4;
            case 4:
              decelVehicle();
              stopVehicle();
              rev_seq = 5;
            case 5:
              //delay(250);
              moveStraight();
              break;
            default:
              stopVehicle();
              break;
          }
      }
    }
    else {
      Serial.print("Start Uninitiated\n");
      stopVehicle();
    }
}

void lightControl(){
  int light_int = analogRead(photocell_pin); //read analog signal from photocell
  delay(500);
  if (light_int < 200){
    digitalWrite(white_led, HIGH);
  }
  else  {
    digitalWrite(white_led, LOW);
  }
}

int distanceSense(){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH); //send pulse
  delayMicroseconds(5);
  digitalWrite(trig_pin, LOW);
  duration = pulseIn(echo_pin, HIGH); //receive echo from pulse
  inches = duration / 74 / 2; //convert pulses time to distance
  //delay(100);
  return inches;
}

void moveVehicle(String dir){
  if (dir == "fwdCmd"){
    digitalWrite(red_led, LOW);
    digitalWrite(motor_dr, LOW);
    for (float i=0; i<=7.2; i=i+0.5){
      dutyCycle = pow(2,i);
      ledcWrite(pwmChannel, dutyCycle);
      delay(100); 
    }
  }
  if (dir == "revCmd"){
    digitalWrite(red_led, LOW);
    digitalWrite(motor_dr, HIGH);
    for (float i=0; i<=8; i++){
      dutyCycle = pow(2,i);
      ledcWrite(pwmChannel, dutyCycle);
      delay(100); 
    }
  }
}

void decelVehicle(){
  digitalWrite(red_led, HIGH);
  for (float i=7; i>0; i--){
    dutyCycle = pow(2,i);
    ledcWrite(pwmChannel, dutyCycle); 
    delay(100);
  } 
}

void stopVehicle(){
  digitalWrite(red_led, HIGH);
  ledcWrite(pwmChannel, 0);
  delay(500);
}

void moveLeft(){
  dutyCycleSteer = 256; // Move DC motor left at a full speed (duty cycle value=256 or 100%)
  digitalWrite(steer_motor_dr, HIGH);
  ledcWrite(pwmChannelSteer, dutyCycleSteer);
  delay(500);
}

void moveRight(){
  dutyCycleSteer = 256; // Move DC motor right at a full speed (duty cycle value=256 or 100%)
  digitalWrite(steer_motor_dr, LOW); 
  ledcWrite(pwmChannelSteer, dutyCycleSteer);
  delay(500);
}

void moveStraight(){
  dutyCycleSteer = 0; // Move DC motor straight with zero speed (duty cycle value=0 or 0%)
  digitalWrite(steer_motor_dr, LOW); 
  ledcWrite(pwmChannelSteer, dutyCycleSteer);
  delay(1000);
}
