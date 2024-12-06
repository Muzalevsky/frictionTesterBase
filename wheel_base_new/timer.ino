extern void control();

void init_timer() {
  // put your setup code here, to run once:
  TCCR1A = 0;  // установить TCCR1A регистр в 0
  TCCR1B = 0;

  // включить прерывание Timer1 overflow:
  TIMSK1 = (1 << TOIE1);
  // Установить CS11 бит так, чтобы таймер работал при тактовой частоте/8:
  TCCR1B |= (1 << CS11);
}

ISR(TIMER1_OVF_vect) {
  control();
}
