#define Left_hallSensorA 2
#define Left_hallSensorC 11

#define Right_hallSensorA 3
#define Right_hallSensorC 12

#define LEFT_SPEED_PULSE 7
#define RIGHT_SPEED_PULSE 8

int Left_count = 0;
float Left_angle = 0;
int Right_count = 0;
float Right_angle = 0;
float k = 1.3846;

bool Left_isClockwise = true;
bool Right_isClockwise = true;

void init_hall() {
  pinMode(Left_hallSensorA, INPUT_PULLUP);
  pinMode(Left_hallSensorC, INPUT);

  pinMode(Right_hallSensorA, INPUT);
  pinMode(Right_hallSensorC, INPUT);

  attachInterrupt(digitalPinToInterrupt(Left_hallSensorA), handleLeft_hallSensorA, RISING);
  attachInterrupt(digitalPinToInterrupt(Right_hallSensorA), handleRight_hallSensorA, RISING);
  //Добавить инит прерывания для правого колеса
  //  attachInterrupt(digitalPinToInterrupt(Left_hallSensorA), handleLeft_hallSensorA2, FALLING);
  //attachInterrupt(digitalPinToInterrupt(Right_hallSensorA), handle_RighthallSensorA, RISING);

  pinMode(LEFT_SPEED_PULSE, INPUT_PULLUP);
  pinMode(RIGHT_SPEED_PULSE, INPUT_PULLUP);

  PCICR |= (1 << PCIE2) | (1 << PCIE0);  // Включаем PCINT для PORTD и PORTB
  PCMSK2 |= (1 << PCINT23);              // Включаем прерывание для D7 (PD7)
  PCMSK0 |= (1 << PCINT0);               // Включаем прерывание для D8 (PB0)
}

void handleLeft_hallSensorA() {
  bool A = digitalRead(Left_hallSensorA);
  bool C = digitalRead(Left_hallSensorC);
  if (A) {
    if (C) {
      Left_isClockwise = true;
    } else {
      Left_isClockwise = false;
    }
  }
}

void handleRight_hallSensorA() {
  bool A = digitalRead(Right_hallSensorA);
  bool C = digitalRead(Right_hallSensorC);
  if (A) {
    if (C) {
      Right_isClockwise = true;
    } else {
      Right_isClockwise = false;
    }
  }
}

ISR(PCINT2_vect) {
  if (Left_isClockwise) {
    Left_count--;
  } else {
    Left_count++;
  }
  Left_angle = Left_count * k;
  //Serial.println(Left_isClockwise);
  //Serial.println(Left_angle);
}


ISR(PCINT0_vect) {
  if (Right_isClockwise) {
    Right_count++;
  } else {
    Right_count--;
  }
  Right_angle = Right_count * k;
  //Serial.println(Right_isClockwise);
  //Serial.println(Right_angle);
}
