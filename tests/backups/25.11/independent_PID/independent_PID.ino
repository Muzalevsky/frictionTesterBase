#define LEFT_SPEED_PULSE 2
#define RIGHT_SPEED_PULSE 3
#define LEFT_AN 5 // PWM pin
#define RIGHT_AN 6 // PWM pin
#define LEFT_CW 7 // HIGH - forward
#define RIGHT_CW 8 // LOW - forward


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

float kp_left=5.1;
float kd_left=0.0001;
float ki_left=0.000001;
uint32_t kob_left = 258;

float kp_right=5.1;
float kd_right=0.0001;
float ki_right=0.000001;
uint32_t kob_right = 261;


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
  Serial.println("left , right");

  TCCR1A = 0; // установить TCCR1A регистр в 0
  TCCR1B = 0;

    // включить прерывание Timer1 overflow:
  TIMSK1 = (1 << TOIE1);
    // Установить CS10 бит так, чтобы таймер работал при тактовой частоте:
  TCCR1B |=  (1 << CS11);

  sei();  // включить глобальные прерывания
}

void leftPulse() {
  gLeftCounter++;  
}

void rightPulse() {
  gRightCounter++;
}

ISR(TIMER1_OVF_vect) 
{
  
  current_speed_left = gLeftCounter-prev_ticks_left; 
  current_speed_right = gRightCounter-prev_ticks_right;

  uint32_t error_left = given_speed_left - current_speed_left;
  uint32_t error_right = given_speed_right - current_speed_right;

  uint32_t error_d_left = error_left - prev_err_left;
  uint32_t error_d_right = error_right - prev_err_right;

  uint32_t error_i_left = error_i_left + error_left;
  uint32_t error_i_right = error_i_right + error_right;
  
  control_left = error_left*kp_left + error_d_left*kd_left + error_i_left*ki_left;
  control_right = error_right*kp_right + error_d_right*kd_right + error_i_right*ki_right;

  prev_err_left = error_left;
  prev_err_right = error_right;

  prev_ticks_left = gLeftCounter;
  prev_ticks_right = gRightCounter;
}

//int speed, int distance
void doMove(uint32_t speed_left,uint32_t speed_right,uint32_t distance_left,uint32_t distance_right)
{
  distance_right = distance_right*kob_right;
  distance_left = distance_left*kob_left;
  // gRightCounter>=distance_right && 
  
  while (!(gRightCounter>=distance_right && gLeftCounter>=distance_left)){
    given_speed_left = speed_left;
    given_speed_right = speed_right;
    digitalWrite(LEFT_CW, HIGH);
    digitalWrite(RIGHT_CW, LOW);
    analogWrite(LEFT_AN, control_left); 
    analogWrite(RIGHT_AN, prev_err_right);
    if (Serial.available() > 1) {
    char key = Serial.read();
    int val = Serial.parseFloat();
    switch (key) {
      case 'p': kp_left = val; break;
      case 'i': ki_left = val; break;
      case 'd': kd_left = val; break;
    }
  }
  Serial.print(gLeftCounter);
  Serial.print(',');
  Serial.println(gRightCounter);
  }
  analogWrite(LEFT_AN, 0); 
  analogWrite(RIGHT_AN, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  doMove(200,200,5,5);

  

}
