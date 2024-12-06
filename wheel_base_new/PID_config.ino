extern float Left_kp;
extern float Left_ki;
extern float Left_kd;

extern float Left_given_angle;
// Функция для парсинга входных данных
void parsePID(String input) {
  int firstSemiColon = input.indexOf(';');
  int secondSemiColon = input.indexOf(';', firstSemiColon + 1);
  int thirdSemiColon = input.indexOf(';', secondSemiColon + 1);

  if (firstSemiColon != -1 && secondSemiColon != -1 && thirdSemiColon != -1) {
    int angle = input.substring(0, firstSemiColon).toInt();
    int P = input.substring(firstSemiColon + 1, secondSemiColon).toInt();
    int I = input.substring(secondSemiColon + 1, thirdSemiColon).toInt();
    int D = input.substring(thirdSemiColon + 1).toInt();
    Left_kp = (float)P / 100;
    Left_ki = (float)I / 100;
    Left_kd = (float)D / 100;

    Left_given_angle = (float)angle;
    // Выводим полученные значения для проверки
    Serial.print("Given Angle: ");
    Serial.println(Left_given_angle);
    Serial.print("P: ");
    Serial.println(Left_kp);
    Serial.print("I: ");
    Serial.println(Left_ki);
    Serial.print("D: ");
    Serial.println(Left_kd);
  }
}