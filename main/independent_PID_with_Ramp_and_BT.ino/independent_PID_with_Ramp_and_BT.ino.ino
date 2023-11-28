#define LEFT_SPEED_PULSE 2
#define RIGHT_SPEED_PULSE 3
#define LEFT_AN 5   // PWM pin
#define RIGHT_AN 6  // PWM pin
#define LEFT_CW 7   // HIGH - forward
#define RIGHT_CW 8  // LOW - forward
#define FORWARD 'F'
#define BACKWARD 'B'
#define LEFT 'L'
#define RIGHT 'R'
#define CIRCLE 'C'
#define CROSS 'X'
#define TRIANGLE 'T'
#define SQUARE 'S'
#define START 'A'
#define PAUSE 'P'


int gLeftTask = 0;
int gRightTask = 0;
int gLeftCounter = 0;
int gRightCounter = 0;

uint32_t prev_err_left = 0;
uint32_t prev_ticks_left = 0;
uint32_t error_i_left = 0;
float control_left;
uint32_t given_speed_left;
uint32_t current_speed_left;

uint32_t prev_err_right = 0;
uint32_t prev_ticks_right = 0;
uint32_t error_i_right = 0;
float control_right;
uint32_t given_speed_right;
uint32_t current_speed_right;

float kp_left = 5.1;
float kd_left = 0.0001;
float ki_left = 0.000001;
uint32_t kob_left = 258;

float kp_right = 5.1;
float kd_right = 0.0001;
float ki_right = 0.000001;
uint32_t kob_right = 261;

int32_t gE;
int32_t gmax_speed_left;
int32_t gmax_speed_right;
int32_t gdistance_left;
int32_t gdistance_right;


void executeCommand(char command) {
  switch (command) {
    case FORWARD:
      move_forward();
      break;
    case BACKWARD:
      move_backward();
      break;
    case LEFT:
      move_left();
      break;
    case RIGHT:
      move_right();
      break;
    default:
      stop();
      break;
  }
}

void move_forward() {
  doMove(5, 200, 200, 10, 10);
}

void move_backward() {
  doMove(5, 200, 200, -10, -10);
}
void move_left() {
  doMove(5, 200, 200, 10, -10);
}
void move_right() {
  doMove(5, 200, 200, -10, 10);
}
void stop() {
  doMove(0, 0, 0, 0, 0);
}

void setTimer2Interrupt(bool enableInterrupt) {
  if (enableInterrupt) {
    TIMSK2 |= (1 << OCIE2A);  // включаем прерывание
    TCNT2 = 0;                // обнуляем таймер
  } else {
    TIMSK2 &= ~(1 << OCIE2A);  // отключаем прерывание
    TCNT2 = 0;                 // обнуляем таймер
  }
}

void RampGenerator() {
  given_speed_left = 0;
  given_speed_right = 0;
  gdistance_right = gdistance_right * kob_right;
  gdistance_left = gdistance_left * kob_left;
  float S_left = gLeftCounter / kob_left;
  float S_right = gRightCounter / kob_right;
  if (given_speed_left < abs(gmax_speed_left)) {
    if (gmax_speed_left >= 0) {
      given_speed_left += gE;
    } else {
      given_speed_left -= gE;
    }
  } else {
    float S_usk_left = gLeftCounter / kob_left;
    given_speed_left = gmax_speed_left;
    if (S_left > gdistance_left - 2 * S_usk_left) {
      if (gmax_speed_left >= 0) {
        given_speed_left -= gE;
      } else {
        given_speed_left += gE;
      }
      if (given_speed_left < 0) {
        given_speed_left = 0;
      }
    }
  }
  if (given_speed_right < abs(gmax_speed_right)) {
    if (gmax_speed_left >= 0) {
      given_speed_right += gE;
    } else {
      given_speed_right -= gE;
    }
  } else {
    float S_usk_right = gRightCounter / kob_right;
    given_speed_right = gmax_speed_right;
    if (S_right > gdistance_right - 2 * S_usk_right) {
      if (gmax_speed_left >= 0) {
        given_speed_right -= gE;
      } else {
        given_speed_right += gE;
      }
      if (given_speed_right < 0) {
        given_speed_right = 0;
      }
    }
  }
  if (given_speed_right == 0 && given_speed_left == 0) {
    setTimer2Interrupt(true);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(LEFT_SPEED_PULSE, INPUT);
  pinMode(RIGHT_SPEED_PULSE, INPUT);
  attachInterrupt(digitalPinToInterrupt(LEFT_SPEED_PULSE), leftPulse, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_SPEED_PULSE), rightPulse, CHANGE);

  pinMode(LEFT_AN, OUTPUT);
  pinMode(RIGHT_AN, OUTPUT);

  pinMode(LEFT_CW, OUTPUT);
  pinMode(RIGHT_CW, OUTPUT);

  Serial.begin(9600);
  Serial.setTimeout(50);
  Serial.flush();
  Serial.println("left");

  TCCR1A = 0;  // установить TCCR1A регистр в 0
  TCCR1B = 0;

  // включить прерывание Timer1 overflow:
  TIMSK1 = (1 << TOIE1);
  // Установить CS10 бит так, чтобы таймер работал при тактовой частоте:
  TCCR1B |= (1 << CS11);

  TCCR2A = 0;
  TCCR2B = 0;
  // включить прерывание Timer2 overflow:
  TIMSK2 = (1 << TOIE2);
  // Установить CS12 и CS10 биты так, чтобы таймер работал при тактовой частоте:
  TCCR2B |= (1 << CS22) | (1 << CS20);

  sei();  // включить глобальные прерывания
}

void leftPulse() {
  gLeftCounter++;
}

void rightPulse() {
  gRightCounter++;
}

ISR(TIMER1_OVF_vect) {

  current_speed_left = gLeftCounter - prev_ticks_left;
  current_speed_right = gRightCounter - prev_ticks_right;

  uint32_t error_left = given_speed_left - current_speed_left;
  uint32_t error_right = given_speed_right - current_speed_right;

  uint32_t error_d_left = error_left - prev_err_left;
  uint32_t error_d_right = error_right - prev_err_right;

  uint32_t error_i_left = error_i_left + error_left;
  uint32_t error_i_right = error_i_right + error_right;

  control_left = error_left * kp_left + error_d_left * kd_left + error_i_left * ki_left;
  control_right = error_right * kp_right + error_d_right * kd_right + error_i_right * ki_right;

  prev_err_left = error_left;
  prev_err_right = error_right;

  prev_ticks_left = gLeftCounter;
  prev_ticks_right = gRightCounter;
}

ISR(TIMER2_OVF_vect) {
  RampGenerator();
}

//int speed, int distance
void doMove(int32_t E, int32_t max_speed_left, int32_t max_speed_right, int32_t distance_left, int32_t distance_right) {
  gE = E;
  gmax_speed_left = max_speed_left;
  if (distance_left < 0) {
    gmax_speed_left = -max_speed_left;
  }
  gmax_speed_right = max_speed_right;
  if (gdistance_right < 0) {
    gmax_speed_right = -max_speed_right;
  }
  gdistance_left = distance_left;
  gdistance_right = distance_right;
  setTimer2Interrupt(true);
  gRightCounter = 0;
  gLeftCounter = 0;
  digitalWrite(LEFT_CW, HIGH);
  digitalWrite(RIGHT_CW, LOW);
  analogWrite(LEFT_AN, control_left);
  analogWrite(RIGHT_AN, control_right);
  Serial.print(gLeftCounter);
  Serial.print(',');
  Serial.println(gRightCounter);
  return 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    char command = Serial.read();
    executeCommand(command);
  }
}
