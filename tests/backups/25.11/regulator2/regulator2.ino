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

float kp_right=1.0;
float kd_right=100;
float ki_right=0.0001;
uint32_t kob_right = 261;




float* RampGenerator (float S, float Vmax, float E,float dt){



  int num_t1 = (int)(tnas / dt) + 1;
  int num_t2 = (int)((t_usk - tnas) / dt) + 1;
  int num_t3 = (int)((t_usk + tnas - t_usk) / dt) + 1;
  int total_size = num_t1 + num_t2 + num_t3;

  float* t = (float*)malloc(total_size * sizeof(float));
  float* v = (float*)malloc(total_size * sizeof(float));
  for (int i = 0; i < num_t1; i++) {
        t[i] = i * dt;
        v[i] = E * t[i];
    }

    for (int i = 0; i < num_t2; i++) {
        t[num_t1 + i] = tnas + i * dt;
        v[num_t1 + i] = Vmax;
    }

    for (int i = 0; i < num_t3; i++) {
        t[num_t1 + num_t2 + i] = t_usk + i * dt;
        v[num_t1 + num_t2 + i] = E * (t_usk + tnas) - E * t[num_t1 + num_t2 + i];
    }
    return v;
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
  uint32_t error_right = gLeftCounter - gRightCounter;

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

  int32_t kob_right = 261;
  S = S*kob_right;
  float tnas = Vmax/E;
  float s_nas1=S-E*tnas*tnas;
  float t_usk = sqrt(s_nas1/E);
  
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
  float* result = RampGenerator(5.5,200.0,5000.0,0.00001);
    int total_size = (float*)malloc(total_size * sizeof(float));
    for (int i = 1; i < total_size; i++) {
        printf("v[%d] = %f\n", i, result[i]);
        if (result[i]==0){
            free(result);
        }
    }
  doMove(200,200,5,5);
}
