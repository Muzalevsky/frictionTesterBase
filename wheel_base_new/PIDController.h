// PIDController.h
#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <stdint.h>

class PIDController {
private:
  float prev_angle;
  float prev_err;
  float error_i;



public:
  float kp;
  float ki;
  float kd;
  PIDController(float kp, float ki, float kd)
    : kp(kp), ki(ki), kd(kd) {
    prev_angle = 0;
    prev_err = 0;
    error_i = 0;
  }

  float update(float given_angle, float angle) {
    float current_angle = angle - prev_angle;
    float error = given_angle - angle;
    float error_d = error - prev_err;
    error_i = error_i + error;
    if (error_i > 400) {
      error_i = 400;
    }
    if (error_i < -400) {
      error_i = -400;
    }
    float control = error * kp + error_d * kd + error_i * ki;

    prev_angle = angle;
    prev_err = error;
    error_i += error;

    return control;
  }
};

#endif
