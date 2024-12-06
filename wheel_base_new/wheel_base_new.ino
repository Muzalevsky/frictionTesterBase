extern void init_hall();
extern void init_timer();
extern void init_wheels();
extern void simple_control();
extern void LeftWheelMove(int speed);
extern void RightWheelMove(int speed);
extern void parsePID(String input);
extern void parseAngle(String input);
extern void sendAngles();

uint8_t mode = 0;  //0 - управление через ПИД по таймеру, 1 - управление по кнопкам, 2 - никакой

extern float Left_given_angle;
extern float Right_given_angle;

volatile bool sendDataFlag = false;

void setup() {
  init_hall();
  init_wheels();
  if (mode == 0) { init_timer(); }
  Serial.begin(9600);
  sei();  // включить глобальные прерывания
}

void loop() {

  //if (!mode){simple_control();}
  //Right_given_angle=-360;
  //Left_given_angle=-360;
  //Serial.println(Right_given_angle);
  //LeftWheelMove(-200);
  //RightWheelMove(-50);
  //RightWheelBackward(50);
  //RightWheelForward(50);


  if (sendDataFlag) {
    sendDataFlag = false;  // Сбрасываем флаг
    sendAngles();            // Отправка данных
  }
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Читаем строку до конца строки
    parseAngle(input);                            // Парсим входные данные
    //sendDataFlag = true;
  }


  /*Для настройки PID одного колеса
  if (Serial.available()) {
        String input = Serial.readStringUntil('\n'); // Читаем строку до конца строки
        parsePID(input); // Парсим входные данные
    }*/
}
