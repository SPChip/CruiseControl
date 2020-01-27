void Display1() {
  LCD.Clear_LCD();

  // значок зарядки
  LCD.drawRect(74, 0, 22, 8, 1);
  LCD.drawRect(72, 2, 3, 4, 1);
  for (int i = 93; i > 95 - (batCharge + 4) / 5; i--) {
    LCD.drawFastVLine(i, 2, 4, 1);
  }

  //% зарядки
  if (batCharge >= 100) {
    LCD.print(72, 10, 1, batCharge);
  }
  else if (batCharge >= 10 && batCharge < 100) {
    LCD.print(78, 10, 1, batCharge);
  }
  else (batCharge < 10) {
    LCD.print(84, 10, 1, batCharge);
  }
  LCD.print(91, 10, 1, "%");

  //температура
  LCD.print(40, 0, 1, escTemp / 10);
  if (int(escTemp / 10) >= 10) {
    LCD.drawCircle(54, 1, 1, 1);
  }
  else if (int(escTemp / 10) >= 0 && int(escTemp / 10) < 10) {
    LCD.drawCircle(48, 1, 1, 1);
  }
  else if (int(escTemp / 10) > -10 && int(escTemp / 10) < 0) {
    LCD.drawCircle(54, 1, 1, 1);
  }
  else (int(escTemp) / 10 <= -10) {
    LCD.drawCircle(60, 1, 1, 1);
  }

  //часы
  LCD.print(0, 0, 1, "12:43");

  //расход
  LCD.simb16x32(0, 15, 1, 0);
  LCD.simb16x32(18, 15, 1, 3);
  LCD.simb16x32(36, 15, 1, 4);
  LCD.simb16x32(54, 15, 1, 2);
  LCD.print(75, 33, 1, "Втч");
  LCD.print(75, 40, 1, "/км");

  //линии
  LCD.drawFastHLine(0, 50, 96, 1);
  LCD.drawFastVLine(45, 50, 18, 1);

  //пробег
  LCD.print(0, 52, 1, "Dist");
  LCD.print(0, 61, 1, (currentMileage % 10000) / 1000);
  LCD.print(6, 61, 1, (currentMileage % 1000) / 100);
  LCD.print(11, 61, 1, ",");
  LCD.print(15, 61, 1, (currentMileage % 100) / 10);
  LCD.print(21, 61, 1, currentMileage % 10);
  LCD.print(27, 61, 1, "км");

  //время в пути
  LCD.print(49, 52, 1, "Time");
  if (int(millis() / 3600000) <= 9) {          //часы
    LCD.print(49, 61, 1, "0");
    LCD.print(55, 61, 1, int(millis() / 3600000));
  }
  else {
    LCD.print(49, 61, 1, int(millis() / 3600000));
  }
  LCD.print(61, 61, 1, ":");

  if (int((millis() % 3600000) / 60000) <= 9) {          //минуты
    LCD.print(67, 61, 1, "0");
    LCD.print(73, 61, 1, int((millis() % 3600000) / 60000));
  }
  else {
    LCD.print(67, 61, 1, int((millis() % 3600000) / 60000));
  }
  LCD.print(79, 61, 1, ":");

  if (int(((millis() % 3600000) % 60000) / 1000) <= 9) {          //секунды
    LCD.print(85, 61, 1, "0");
    LCD.print(91, 61, 1, int(((millis() % 3600000) % 60000) / 1000));
  }
  else {
    LCD.print(85, 61, 1, int(((millis() % 3600000) % 60000) / 1000));
  }


  LCD.Update();
}

void Display4() {                    // информация о батареях
  LCD.Clear_LCD();
  // заголовок
  LCD.print(12, 0, 1, "Battery info");
  LCD.drawFastHLine(0, 10, 96, 1);
  LCD.print(30, 15, 1, "InBat ExBat");

  // осталась емкость
  LCD.print(0, 28, 1, "Cap");
  if (inBatCapacityLeft >= 1000) {
    LCD.print(36, 28, 1, inBatCapacityLeft);
  }
  else if (inBatCapacityLeft >= 100 && inBatCapacityLeft < 1000) {
    LCD.print(36, 28, 1, " ");
    LCD.print(42, 28, 1, inBatCapacityLeft);
  }
  else if (inBatCapacityLeft >= 10 && inBatCapacityLeft < 100) {
    LCD.print(36, 28, 1, "  ");
    LCD.print(48, 28, 1, inBatCapacityLeft);
  }
  else (inBatCapacityLeft < 10) {
    LCD.print(36, 28, 1, "   ");
    LCD.print(54, 28, 1, inBatCapacityLeft);
  }


  if (exBatCapacityLeft >= 1000) {
    LCD.print(73, 28, 1, exBatCapacityLeft);
  }
  else if (exBatCapacityLeft >= 100 && exBatCapacityLeft < 1000) {
    LCD.print(73, 28, 1, " ");
    LCD.print(79, 28, 1, exBatCapacityLeft);
  }
  else if (exBatCapacityLeft >= 10 && exBatCapacityLeft < 100) {
    LCD.print(73, 28, 1, "  ");
    LCD.print(85, 28, 1, exBatCapacityLeft);
  }
  else {
    LCD.print(73, 28, 1, "   ");
    LCD.print(91, 28, 1, exBatCapacityLeft);
  }


  // заряд
  LCD.print(0, 39, 1, "Chrg");

  if (inBatCharge >= 100) {
    LCD.print(36, 39, 1, inBatCharge);
  }
  else if (inBatCharge >= 10 && inBatCharge < 100) {
    LCD.print(42, 39, 1, inBatCharge);
  }
  else (inBatCharge < 10) {
    LCD.print(48, 39, 1, inBatCharge);
  }

  LCD.print(54, 39, 1, "%");
  if (exBatCharge >= 100) {
    LCD.print(72, 39, 1, exBatCharge);
  }
  else if (exBatCharge >= 10 && exBatCharge < 100) {
    LCD.print(78, 39, 1, exBatCharge);
  }
  else (exBatCharge < 10) {
    LCD.print(84, 39, 1, exBatCharge);
  }
  LCD.print(91, 39, 1, "%");

  // ток
  LCD.print(0, 50, 1, "Cur");

  if (inBatCurent  >= 1000) {
    LCD.print(36, 50, 1, (inBatCurent % 10000) / 1000);
  }
  else {
    LCD.print(36, 50, 1, " ";
  }
  LCD.print(42, 50, 1, (inBatCurent % 1000) / 100);
  LCD.print(47, 50, 1, ",");
  LCD.print(51, 50, 1, (inBatCurent % 100) / 10);
  LCD.print(57, 50, 1, inBatCurent % 10);
  LCD.print(63, 50, 1, "A");

  if (exBatCurent  >= 1000) {
    LCD.print(73, 50, 1, (exBatCurent % 10000) / 1000);
  }
  else {
    LCD.print(73, 50, 1, " ";
  }
  LCD.print(79, 50, 1, (exBatCurent % 1000) / 100);
  LCD.print(84, 50, 1, ",");
  LCD.print(88, 50, 1, (exBatCurent % 100) / 10);
  LCD.print(94, 50, 1, exBatCurent % 10);
  LCD.print(100, 50, 1, "A");




  // напряжение
  LCD.print(0, 61, 1, "Volt");

  LCD.print(36, 61, 1, (inBatVoltage % 10000) / 1000);
  LCD.print(42, 61, 1, (inBatVoltage % 1000) / 100);
  LCD.print(47, 61, 1, ",");
  LCD.print(51, 61, 1, (inBatVoltage % 100) / 10);
  LCD.print(57, 61, 1, inBatVoltage % 10);
  LCD.print(63, 61, 1, "V");

  LCD.print(73, 61, 1, (exBatVoltage % 10000) / 1000);
  LCD.print(79, 61, 1, (exBatVoltage % 1000) / 100);
  LCD.print(84, 61, 1, ",");
  LCD.print(88, 61, 1, (exBatVoltage % 100) / 10);
  LCD.print(94, 61, 1, exBatVoltage % 10);
  LCD.print(100, 61, 1, "V");

  LCD.Update();
}













void DisplayLogo() {
  LCD.Clear_LCD();
  LCD.print(26, 5, 1, "NINEBOT");
  LCD.print(6, 17, 1, "CRUISE CONTROL");
  LCD.print(21, 40, 1, "by SPChip");
  LCD.print(1, 60, 1, "ver 1.0");
  LCD.Update();
}
