float Left_given_angle;
float Right_given_angle;
// Функция для парсинга входных данных
void parseAngle(String input) {
  int firstSemiColon = input.indexOf(';');
    int lleft_angle = input.substring(0, firstSemiColon).toInt();
    int lright_angle = input.substring(firstSemiColon+1).toInt();

    Left_given_angle = (float)lleft_angle;
    Right_given_angle = (float)lright_angle;
    // Выводим полученные значения для проверки
    
}