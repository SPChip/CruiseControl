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
  if (batCharge >= 10 && batCharge < 100) {
    LCD.print(78, 10, 1, batCharge);
  }
  if (batCharge < 10) {
    LCD.print(84, 10, 1, batCharge);
  }
  LCD.print(91, 10, 1, "%");

  //температура
  LCD.print(40, 0, 1, escTemp / 10);
  if (int(escTemp) / 10 <= -10) {
    LCD.drawCircle(60, 1, 1, 1);
  }
  if (int(escTemp / 10) > -10 && int(escTemp / 10) < 0) {
    LCD.drawCircle(54, 1, 1, 1);
  }
  if (int(escTemp / 10) >= 0 && int(escTemp / 10) < 10) {
    LCD.drawCircle(48, 1, 1, 1);
  }
  if (int(escTemp / 10) >= 10) {
    LCD.drawCircle(54, 1, 1, 1);
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

void Display4() {
  LCD.Clear_LCD();
  LCD.print(12, 0, 1, "Battery info");
  LCD.drawFastHLine(0, 10, 96, 1);
  LCD.print(30, 15, 1, "InBat ExBat");

  LCD.print(0, 28, 1, "Cap");
  LCD.print(36, 28, 1, inBatCapacityLeft);
  LCD.print(73, 28, 1, exBatCapacityLeft);


  LCD.print(0, 39, 1, "Chrg");
  if (inBatCharge >= 100) {
    LCD.print(36, 39, 1, inBatCharge);
  }
  if (inBatCharge >= 10 && inBatCharge < 100) {
    LCD.print(42, 39, 1, inBatCharge);
  }
  if (inBatCharge < 10) {
    LCD.print(48, 39, 1, inBatCharge);
  }
  LCD.print(54, 39, 1, "%");
  if (exBatCharge >= 100) {
    LCD.print(72, 39, 1, exBatCharge);
  }
  if (exBatCharge >= 10 && exBatCharge < 100) {
    LCD.print(78, 39, 1, exBatCharge);
  }
  if (exBatCharge < 10) {
    LCD.print(84, 39, 1, exBatCharge);
  }
  LCD.print(91, 39, 1, "%");




  LCD.print(0, 50, 1, "Cur");
  LCD.print(0, 61, 1, "Volt");
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




