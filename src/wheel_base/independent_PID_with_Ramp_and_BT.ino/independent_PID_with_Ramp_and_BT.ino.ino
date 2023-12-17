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

int32_t prev_err_left = 0;
uint32_t prev_ticks_left = 0;
int32_t error_i_left = 0;
uint8_t control_left;
int32_t given_speed_left;
int32_t current_speed_left;

int32_t prev_err_right = 0;
uint32_t prev_ticks_right = 0;
int32_t error_i_right = 0;
uint8_t control_right;
int32_t given_speed_right;
int32_t current_speed_right;

float kp_left = 3;
float kd_left = 0;
float ki_left = 0.01;
uint32_t kob_left = 258;

float kp_right = 3;
float kd_right = 0;
float ki_right = 0.01;
uint32_t kob_right = 261;

int32_t gE;
int32_t gmax_speed_left;
int32_t gmax_speed_right;
int32_t gdistance_left;
int32_t gdistance_right;

int32_t error_left = 0;
int32_t error_right = 0;

void executeCommand(char command) {
  switch (command) {
    case FORWARD:
      Serial.println("go");
      move_forward();
      break;
    case BACKWARD:
    //Serial.println("BACK");
      move_backward();
      break;
    case LEFT:
      move_left();
      break;
    case RIGHT:
      move_right();
      break;
    default:
      stop_move();
      break;
  }
}

void move_forward() {
  doMove(20, 20, 50, 50, 1);
}

void move_backward() {
  doMove(10, 10, -50, -50, -1);
}
void move_left() {
  doMove(20, -20, 50, 50, 1);
}
void move_right() {
  doMove(-20, 20, 50, 50, 1);
}
void stop_move() {
  analogWrite(LEFT_AN, 0);
  analogWrite(RIGHT_AN, 0);
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

bool accelerating_left = false;
bool decelerating_left = false;
bool accelerating_right = false;
bool decelerating_right = false;
int32_t gAccelaration;
void RampGenerator() {
  if (accelerating_left) {
    given_speed_left += gAccelaration;
  }
  if (decelerating_left) {
    given_speed_left -= gAccelaration;
  }
  if (accelerating_right) {
    given_speed_right += gAccelaration;
  }
  if (decelerating_right) {
    given_speed_right -= gAccelaration;
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
  //Serial.flush();
  //Serial.println("left");

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

  error_left = given_speed_left - current_speed_left;
  error_right = given_speed_right - current_speed_right;

  int32_t error_d_left = error_left - prev_err_left;
  int32_t error_d_right = error_right - prev_err_right;

  error_i_left = error_i_left + error_left;
  error_i_right = error_i_right + error_right;
  if (error_i_left > 1000) {
    error_i_left = 1000;
  }
  if (error_i_right > 1000) {
    error_i_right = 1000;
  }
  int32_t lcontrol_left = error_left * kp_left + error_d_left * kd_left + error_i_left * ki_left;
  int32_t lcontrol_right = error_right * kp_right + error_d_right * kd_right + error_i_right * ki_right;

  if (lcontrol_left <= 0) {
    //reverse
    digitalWrite(LEFT_CW, LOW);
    lcontrol_left = -lcontrol_left;
  } else {
    digitalWrite(LEFT_CW, HIGH);
  }
  if (lcontrol_right <= 0) {
    //reverse
    lcontrol_right = -lcontrol_right;
    digitalWrite(RIGHT_CW, HIGH);
  } else {
    digitalWrite(RIGHT_CW, LOW);
  }


  if (lcontrol_left > 255) {
    control_left = 255;
  } else {
    control_left = lcontrol_left;
  }
  if (lcontrol_right > 255) {
    control_right = 255;
  } else {
    control_right = lcontrol_right;
  }


  prev_err_left = error_left;
  prev_err_right = error_right;

  prev_ticks_left = gLeftCounter;
  prev_ticks_right = gRightCounter;
}

uint32_t countfortimer = 0;
unsigned long currentTime;
unsigned long lastTime;
float timeDelta;
ISR(TIMER2_OVF_vect) {
  if (countfortimer % 100 == 0) {
    currentTime = millis();
    timeDelta = (currentTime - lastTime) / 1000.0;  // разница времени в секундах
    lastTime = currentTime;
    RampGenerator();
  }
  countfortimer++;
}

//int speed, int distance
void doMove(int32_t distance_left, int32_t distance_right, int32_t speed_left, int32_t speed_right, int32_t E) {
  gAccelaration = E;
  distance_left = distance_left * kob_left;
  uint32_t S_usk_left = 1500;
  if (gLeftCounter < S_usk_left) {
    accelerating_left = true;
    decelerating_left = false;
  } else {
    accelerating_left = false;
  }
  if (gLeftCounter > distance_left - S_usk_left) {
    decelerating_left = true;
    if (given_speed_left >= 0) {
      given_speed_left = 9;
      accelerating_left = false;
      decelerating_left = false;
      //setTimer2Interrupt(false);
    }
  }
  distance_right = distance_right * kob_right;
  uint32_t S_usk_right = 1500;
  if (gRightCounter < S_usk_right) {
    accelerating_right = true;
    decelerating_right = false;
  } else {
    accelerating_right = false;
  }
  if (gRightCounter > distance_right - S_usk_right) {
    decelerating_right = true;
    if (given_speed_right >= 0) {//>= - reverse  <= - !reverse
      given_speed_right = 9; // почти стоп (очень медленно) 
      accelerating_right = false;
      decelerating_right = false;
      //setTimer2Interrupt(false);
    }
  }
  
  analogWrite(LEFT_AN, control_left);
  analogWrite(RIGHT_AN, control_right);
  Serial.print(control_left);
  Serial.print(',');
  Serial.println(control_right);
  return 0;
}
char gcommand;
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    char command = Serial.read();
    gcommand=command;
  }
  executeCommand(gcommand);
}
