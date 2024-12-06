#include "PIDController.h"

extern float Left_angle;
extern float Left_given_angle;

float Left_kp = 0.7;
float Left_ki = 0.03;
float Left_kd = 0.05;
int32_t control_left;

extern float Right_angle;
extern float Right_given_angle;

float Right_kp = 0.7;
float Right_ki = 0.03;
float Right_kd = 0.05;
int32_t control_right;

extern volatile bool sendDataFlag;

PIDController pid_left(Left_kp, Left_ki, Left_kd);
PIDController pid_right(Right_kp, Right_ki, Right_kd);

void control() {

  pid_left.kp = Left_kp;
  pid_left.ki = Left_ki;
  pid_left.kd = Left_kd;

  pid_right.kp = Left_kp;
  pid_right.ki = Left_ki;
  pid_right.kd = Left_kd;

  float lcontrol_left = pid_left.update(Left_given_angle, Left_angle);
  float lcontrol_right = pid_left.update(Right_given_angle, Right_angle);


  //ограничение управляющего сигнала
  if (lcontrol_left > 255) {
    control_left = 255;
  } else if (lcontrol_left < -255) {
    control_left = -255;
  } else {
    control_left = lcontrol_left;
  }
  //ограничение управляющего сигнала
  if (lcontrol_right > 255) {
    control_right = 255;
  } else if (lcontrol_right < -255) {
    control_right = -255;
  } else {
    control_right = lcontrol_right;
  }


  LeftWheelMove(control_left);
  RightWheelMove(control_right);
  sendDataFlag = true;
}

void sendAngles() {
  //unsigned long currentTime = millis();  // Получаем текущее время в миллисекундах

  //Serial.print("T:");
  //Serial.print(currentTime);
  Serial.print("L:");
  Serial.print(Left_angle);
  Serial.print(";R:");
  Serial.println(Right_angle);
}
