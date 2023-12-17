volatile int counter = 0;  // переменная-счётчик
volatile int counter2 = 0;

boolean recievedFlag;
String strData = "";
int DATANum = 0;
char Buffer2[50];
int ResultCommadTranslation;
int ResultCommadTranslation2;
volatile int counterRIGHT;  // переменная-счётчик Правый мотор
volatile int counterLEFT; // переменная-счётчик Левый мотор
int LEFTMdist = 0;
int RIGHTMdist = 0;

int LEFTMdist2 = 0;
int RIGHTMdist2 = 0;

int pastCounter = 0;
int pastCounter2 = 0;

int ENABLEtoCOMMAND;

void setup() {
  Serial.begin(115200); // открыли порт для связи
  // подключили кнопку на D2 и GND
  pinMode(2, INPUT_PULLUP); // RIGHT
  pinMode(3, INPUT_PULLUP); // LEFT
  // FALLING - при нажатии на кнопку будет сигнал 0, его и ловим
  attachInterrupt(0, btnIsr, FALLING);
  attachInterrupt(1, btnIsr2, FALLING);
}
void btnIsr() {
  counter++;  // + нажатие
}

void btnIsr2() {
  counter2++;  // + нажатие
}

void loop() {

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
      delay(10);
      Serial.println(); 
      Serial.println((Buffer2[2])); 

            

    }

  if ((Buffer2[0]) == char('1'))
  {
    // Вперёд ОБА МОТОРА
    ENABLEtoCOMMAND == 0;
    pinMode(7, LOW); // Normal rotation
    pinMode(9, LOW); // Normal rotation
    LEFTMdist = (Buffer2[1] - '0') * 100;
    LEFTMdist = LEFTMdist + ((Buffer2[2] - '0') * 10);
    LEFTMdist = LEFTMdist + (Buffer2[3] - '0');
    RIGHTMdist = LEFTMdist;
    
    // Serial.println((LEFTMdist) + String(" LeftDist"));
    // RIGHTMdist = (Buffer2[4] - '0') * 100;
    // RIGHTMdist = RIGHTMdist + ((Buffer2[5]) * 10);
    // RIGHTMdist = RIGHTMdist + (Buffer2[6]);

    Serial.println(String(LEFTMdist) + String(" LeftDist") + String(RIGHTMdist) + String(" RightDist"));
    delay(300);
  }

  if (ENABLEtoCOMMAND == 0)
  {
    ResultCommadTranslation = getStraightTranslation(LEFTMdist, RIGHTMdist);
    if (ResultCommadTranslation == 1)
    {
      ResultCommadTranslation=0;
      Buffer2[0] = {" "};
    }
  }
  Serial.print(" - " + String(RIGHTMdist2) + " RIGHT" + " + " + String(LEFTMdist2) + " LEFT" + String(pastCounter) + " R " + String(pastCounter2) + " L");
  Serial.println();
}



int getLEFT(int LeftTranslation)
{
  Serial.println(String(LeftTranslation) + " getLEFT");
  if (counter2 < LeftTranslation)
  {
    // левый - полный вперед
    pinMode(5, HIGH); // On LEFT M
    pinMode(6, HIGH); // Enabled LEFT M
    pinMode(7, HIGH);
  }  
  else
  {
    counter2 = 0;
    LEFTMdist2 = 0;
    LEFTMdist = 0;
    pinMode(5, LOW); // On LEFT M
    pinMode(6, LOW); // Enabled LEFT M
    pinMode(7, LOW);
    return(1);
  }
}

int getRIGHT(int RightTranslation)
{
  Serial.println(String(RightTranslation) + " getRIGHT");
  if (counter < RightTranslation)
  {
    // Правый мотор
    pinMode(4, HIGH); // On RIGHT M
    pinMode(8, HIGH); // Enabled LEFT M
    pinMode(9, HIGH);
  }  
  else
  {
    counter = 0;
    RIGHTMdist2 = 0;
    RIGHTMdist = 0;
    pinMode(4, LOW); // On RIGHT M
    pinMode(8, LOW); // Enabled RIGHT M
    pinMode(9, LOW);
    return(1);
  }
}

// Функция перемещения принимает команду на количество прерываний на мотор и выполняет ее
int getCommandTranslation(int RightTranslation, int LeftTranslation)
{

  // Как только пришла команда на перемещение - сбросить считанные раньше перемещения
  counterLEFT = 0;
  counterRIGHT = 0;
  // Serial.println((LeftTranslation) + String(" LeftTranslation"));

  if (counter2 < LeftTranslation)
  {
    if (counter < RightTranslation)
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
    if (counter < RightTranslation)
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
      RIGHTMdist = 0;
      LEFTMdist = 0;

      LEFTMdist2 = LEFTMdist2 + counter2;
      RIGHTMdist2 = RIGHTMdist2 + counter;

      counter = 0;
      counter2 = 0;
      pinMode(4, LOW); // Off LEFT M
      pinMode(6, LOW); // Disabled LEFT M
      pinMode(5, LOW); // Off RIGHT M
      pinMode(8, LOW); // Disabled RIGHT M
      return(1);
      // В данном случае считаем что все моторы прибыли на свои позиции
    
    } 
  }
}

// Функция перемещения принимает команду на количество прерываний на оба мотора и выполняет ее
int getStraightTranslation(int RightTranslation, int LeftTranslation)
{
  // Как только пришла команда на перемещение - сбросить считанные раньше перемещения
  counter = 0;
  counter2 = 0;
  if (counter2 < LeftTranslation)
  {
    // Все моторы - полный вперед
    pinMode(4, HIGH); // On LEFT M
    pinMode(6, HIGH); // Enabled LEFT M
    pinMode(5, HIGH); // On RIGHT M
    pinMode(8, HIGH); // Enabled RIGHT M
  }
  else
  {
    // Все моторы на месте, полный стоп
    LEFTMdist2 = LEFTMdist2 + counter2;
    RIGHTMdist2 = RIGHTMdist2 + counter;
    pastCounter = RIGHTMdist;
    pastCounter2 = LEFTMdist;
    RIGHTMdist = 0;
    LEFTMdist = 0;
    pinMode(4, LOW); // Off LEFT M
    pinMode(6, LOW); // Disabled LEFT M
    pinMode(5, LOW); // Off RIGHT M
    pinMode(8, LOW); // Disabled RIGHT M
    return(1);
    // В данном случае считаем что все моторы прибыли на свои позиции
  }
}
