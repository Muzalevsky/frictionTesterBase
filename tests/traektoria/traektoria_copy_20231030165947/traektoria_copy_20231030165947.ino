#define HC_RIGHT_TRIG 11 // Датчик дальности триггер
#define HC_RIGHT_ECHO 10 // Датчик дальности эхолот

volatile int counterRIGHT;  // переменная-счётчик Правый мотор
volatile int counterLEFT; // переменная-счётчик Левый мотор

int RightTranslation; // Количество прерываний на правый мотор задаём по заданию
int LeftTranslation;  // Количество прерывания на левый мотор задаём по заданию

int SummaryRight = 0; // Общий счётчик перемещений Правого мотора
int SummaryLeft = 0;  // Общий счётчик перемещений Левого мотора

String strData = "";
char Buffer2[50];
boolean recievedFlag;
int DATANum = 0;
int LEFTMdist = 0;

int ResultCommadTranslation;


void setup() {

  // подключили на D2 и D3 счет оборотов моторов
  pinMode(2, INPUT_PULLUP); // RIGHT
  pinMode(3, INPUT_PULLUP); // LEFT

  pinMode(4, OUTPUT); // On/Off  LEFT MOTOR
  pinMode(6, OUTPUT); // Enabled LEFT MOTOR
  pinMode(7, OUTPUT); // Reverse LEFT MOTOR
  
  pinMode(4, LOW); // Off LEFT M
  pinMode(6, LOW); // Disabled LEFT M
  pinMode(7, LOW); // Normal rotation

  pinMode(5, OUTPUT); // On/Off  RIGHT MOTOR
  pinMode(8, OUTPUT); // Enabled RIGHT MOTOR
  pinMode(9, OUTPUT); // Reverse RIGHT MOTOR

  pinMode(5, LOW); // Off RIGHT M
  pinMode(8, LOW); // Disabled RIGHT M
  pinMode(9, LOW); // Normal rotation

  pinMode(HC_RIGHT_TRIG, OUTPUT); // trig выход
  pinMode(HC_RIGHT_ECHO, INPUT);  // echo вход
  Serial.begin(115200); // открыли порт для связи

  // FALLING - при нажатии на кнопку будет сигнал 0, его и ловим
  pinMode(2, INPUT_PULLUP); // RIGHT
  pinMode(3, INPUT_PULLUP); // LEFT
  attachInterrupt(0, btnIsr, FALLING);
  attachInterrupt(1, btnIsr2, FALLING);
  
}


void btnIsr() {
  counterRIGHT++;  // + нажатие
}

void btnIsr2() {
  counterLEFT++;  // + нажатие
}


void loop() {  
  float dist = getDist();   // получаем расстояние
  // Serial.println(String(dist) + " препятствие " + String(counterRIGHT) + " правый мотор " + String(counterLEFT) + " левый мотор ");     // выводим

  
  
  // Если впереди свободно то
  if (dist > 0.05)
  {

    // Для поездки по точкам нужно казать в прерываниях сколько крутиться моторам
    // 1 метр 
    // 205 206 207 прерываний
    // 
    // 50 см
    // 103 прерывания
    // 
    // 10 см
    // 21 22 прерывания  
    // 
    // ТАКЖЕ ТРЕБУЕТСЯ УКАЗАТЬ НАПРАВЛЕНИЕ ДВИЖЕНИЯ
    //
    // После указания расстояния в прерываниях для мотора
    // Требуется вызвать функцию отработки Команды
        
    // counterRIGHT = 207;
    // counterLEFT = 207;

    while (Serial.available() > 0)      // ПОКА есть что то на вход 
    {            
      strData += (char)Serial.read();        // забиваем строку принятыми данными
      recievedFlag = true;                   // поднять флаг что получили данные
      delay(2);                              // ЗАДЕРЖКА. Без неё работает некорректно!
    }
    
    if (recievedFlag)       // если данные получены
    {                      
      Serial.println(strData);               // вывести
      Serial.println("I've got it");         // вывести
      DATANum = strData.toInt();
      strData.toCharArray(Buffer2, 50);
      
      strData = "";                          // очистить
      recievedFlag = false;                  // опустить флаг
      Serial.println();
      Serial.print(char(Buffer2) + String(" - buffer"));         // вывести     
      Serial.println(); 
      Serial.println((Buffer2[2])); 

            

    }

        
    // Далее проверяем если пришла команда то
    // 1 направление вперед
    // 2 назад
    // в дециметрах указать расстояние для левого и правого моторов (до 99)
    // 12020 - веперд оба мотора на 20 дециметров
    
    
    if ((Buffer2[0]) == char('1'))
    {
      // Вперёд
      pinMode(7, LOW); // Normal rotation
      pinMode(9, LOW); // Normal rotation
      LEFTMdist = (Buffer2[1] - '0') * 10;
      LEFTMdist = LEFTMdist + (Buffer2[2] - '0');
      
      Serial.println((LEFTMdist) + String(" LeftDist"));
      
    }
    
    
    // ResultCommadTranslation = getCommandTranslation(0, LEFTMdist);
    // if (ResultCommadTranslation == 1)
    // {
    //   // Serial.println("DONE");
    //   ResultCommadTranslation=0;
    // }
    ResultCommadTranslation = getLEFT(LEFTMdist);
    if (ResultCommadTranslation == 1)
    {
      Serial.println(counterLEFT);
      ResultCommadTranslation=0;
      Buffer2[0] = {" "};
      LEFTMdist=0;
    }
    
  }


    

  else
  {
    // Stop all motors
    pinMode(4, LOW); // Off LEFT M
    pinMode(6, LOW); // Disabled LEFT M
    pinMode(7, LOW); // Normal rotation

    pinMode(5, LOW); // Off RIGHT M
    pinMode(8, LOW); // Disabled RIGHT M
    pinMode(9, LOW); // Normal rotation
  }

}






// сделаем функцию для удобства
// Опрос на препятстивие по датчику
float getDist() {
  // импульс 10 мкс
  digitalWrite(HC_RIGHT_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_RIGHT_TRIG, LOW);
  // измеряем время ответного импульса
  uint32_t us = pulseIn(HC_RIGHT_ECHO, HIGH);
  // считаем расстояние и возвращаем
  return (us / 58.2);
}


int getLEFT(int LeftTranslation)
{
  // for (counterLEFT = 0; counterLEFT < LeftTranslation;)
  if (counterLEFT < LeftTranslation)
  {
    // Правый мотор на месте, левый - полный вперед
    pinMode(5, HIGH); // On LEFT M
    pinMode(6, HIGH); // Enabled LEFT M
    pinMode(7, HIGH);
    // pinMode(5, LOW);  // Off RIGHT M
    // pinMode(8, LOW);  // Disabled RIGHT M
  }  
  else
  {
    counterLEFT = 0;

    return(1);
  }
}


// Функция перемещения принимает команду на количество прерываний на мотор и выполняет ее
int getCommandTranslation(int RightTranslation, int LeftTranslation)
{

  // Как только пришла команда на перемещение - сбросить считанные раньше перемещения
  // counterLEFT = 0;
  counterRIGHT = 0;
  // Serial.println((LeftTranslation) + String(" LeftTranslation"));

  if (counterLEFT < LeftTranslation)
  {
    if (counterRIGHT < RightTranslation)
    {
      // Все моторы - полный вперед
      pinMode(4, HIGH); // On LEFT M
      pinMode(6, HIGH); // Enabled LEFT M
      pinMode(5, HIGH); // On RIGHT M
      pinMode(8, HIGH); // Enabled RIGHT M
    }
    else
    {
      // Правый мотор на месте, левый - полный вперед
      pinMode(4, HIGH); // On LEFT M
      pinMode(6, HIGH); // Enabled LEFT M
      pinMode(5, LOW);  // Off RIGHT M
      pinMode(8, LOW);  // Disabled RIGHT M
    }
  }
  else
  {
    // Левый мотор на месте
    if (counterRIGHT < RightTranslation)
    {
      // Левый мотор на месте, правый - полный вперед
      pinMode(4, LOW);  // Off LEFT M
      pinMode(6, LOW);  // Disabled LEFT M
      pinMode(5, HIGH); // On RIGHT M
      pinMode(8, HIGH); // Enabled RIGHT M
    }
    else
    {
      // Все моторы на месте, полный стоп
      pinMode(4, LOW); // Off LEFT M
      pinMode(6, LOW); // Disabled LEFT M
      pinMode(5, LOW); // Off RIGHT M
      pinMode(8, LOW); // Disabled RIGHT M
      return(1);
      // В данном случае считаем что все моторы прибыли на свои позиции
    
    } 
  }
}