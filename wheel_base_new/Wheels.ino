#define LEFT_AN 5    // PWM pin
#define RIGHT_AN 6   // PWM pin
#define LEFT_CW 9    // HIGH - forward
#define RIGHT_CW 10  // LOW - forward


void init_wheels() {
  pinMode(LEFT_AN, OUTPUT);
  pinMode(RIGHT_AN, OUTPUT);

  pinMode(LEFT_CW, OUTPUT);
  pinMode(RIGHT_CW, OUTPUT);
}

void LeftWheelMove(int speed) {
  if (speed < 0) {
    digitalWrite(LEFT_CW, LOW);
  } else {
    digitalWrite(LEFT_CW, HIGH);
  }
  analogWrite(LEFT_AN, abs(speed));
}

void RightWheelMove(int speed) {
  if (speed < 0) {
    digitalWrite(RIGHT_CW, HIGH);
  } else {
    digitalWrite(RIGHT_CW, LOW);
  }
  analogWrite(RIGHT_AN, abs(speed));
}
