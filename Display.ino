
void Display5() {
  LCD.Clear_LCD();
  LCD.print(15, 0, 1, "INBAT CELLs");
  LCD.drawFastHLine(0, 8, 96, 1);
  LCD.print(0, 13, 1, "0");
  LCD.print(0, 25, 1, "1");
  LCD.print(0, 37, 1, "2");
  LCD.print(0, 49, 1, "3");
  LCD.print(0, 61, 1, "4");
  byte stringCell = 13;
  for (int i = 0; i < 5; i++) {
    LCD.print(11, stringCell, 1, inBatCell[i] / 1000);
    LCD.print(16, stringCell, 1, ",");
    LCD.print(20, stringCell, 1, inBatCell[i] % 1000);
    LCD.print(38, stringCell, 1, "V");
    stringCell = stringCell + 12;
  }
  LCD.print(53, 13, 1, "5");
  LCD.print(53, 25, 1, "6");
  LCD.print(53, 37, 1, "7");
  LCD.print(53, 49, 1, "8");
  LCD.print(53, 61, 1, "9");
  stringCell = 13;
  for (int i = 5; i < 10; i++) {
    LCD.print(64, stringCell, 1, inBatCell[i] / 1000);
    LCD.print(69, stringCell, 1, ",");
    LCD.print(73, stringCell, 1, inBatCell[i] % 1000);
    LCD.print(91, stringCell, 1, "V");
    stringCell = stringCell + 12;
  }
  LCD.Update();
}

void Display6() {
  LCD.Clear_LCD();
  LCD.print(15, 0, 1, "EXBAT CELLs");
  LCD.drawFastHLine(0, 8, 96, 1);
  LCD.print(0, 13, 1, "0");
  LCD.print(0, 25, 1, "1");
  LCD.print(0, 37, 1, "2");
  LCD.print(0, 49, 1, "3");
  LCD.print(0, 61, 1, "4");
  byte stringCell = 13;
  for (int i = 0; i < 5; i++) {
    LCD.print(11, stringCell, 1, exBatCell[i] / 1000);
    LCD.print(16, stringCell, 1, ",");
    LCD.print(20, stringCell, 1, exBatCell[i] % 1000);
    LCD.print(38, stringCell, 1, "V");
    stringCell = stringCell + 12;
  }
  LCD.print(53, 13, 1, "5");
  LCD.print(53, 25, 1, "6");
  LCD.print(53, 37, 1, "7");
  LCD.print(53, 49, 1, "8");
  LCD.print(53, 61, 1, "9");
  stringCell = 13;
  for (int i = 5; i < 10; i++) {
    LCD.print(64, stringCell, 1, exBatCell[i] / 1000);
    LCD.print(69, stringCell, 1, ",");
    LCD.print(73, stringCell, 1, exBatCell[i] % 1000);
    LCD.print(91, stringCell, 1, "V");
    stringCell = stringCell + 12;
  }
  LCD.Update();
}






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
  else  {
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
  else  {
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
  LCD.drawFastHLine(0, 8, 96, 1);
  LCD.print(31, 11, 1, "InBat ExBat");

  // осталась емкость
  LCD.print(0, 21, 1, "Cap");
  if (inBatCapacityLeft >= 1000) {
    LCD.print(37, 21, 1, inBatCapacityLeft);
  }
  else if (inBatCapacityLeft >= 100 && inBatCapacityLeft < 1000) {
    LCD.print(37, 21, 1, " ");
    LCD.print(43, 21, 1, inBatCapacityLeft);
  }
  else if (inBatCapacityLeft >= 10 && inBatCapacityLeft < 100) {
    LCD.print(37, 21, 1, "  ");
    LCD.print(49, 21, 1, inBatCapacityLeft);
  }
  else  {
    LCD.print(37, 21, 1, "   ");
    LCD.print(55, 21, 1, inBatCapacityLeft);
  }


  if (exBatCapacityLeft >= 1000) {
    LCD.print(73, 21, 1, exBatCapacityLeft);
  }
  else if (exBatCapacityLeft >= 100 && exBatCapacityLeft < 1000) {
    LCD.print(73, 21, 1, " ");
    LCD.print(79, 21, 1, exBatCapacityLeft);
  }
  else if (exBatCapacityLeft >= 10 && exBatCapacityLeft < 100) {
    LCD.print(73, 21, 1, "  ");
    LCD.print(85, 21, 1, exBatCapacityLeft);
  }
  else {
    LCD.print(73, 21, 1, "   ");
    LCD.print(91, 21, 1, exBatCapacityLeft);
  }


  // заряд
  LCD.print(0, 31, 1, "Chrg");

  if (inBatCharge >= 100) {
    LCD.print(37, 31, 1, inBatCharge);
  }
  else if (inBatCharge >= 10 && inBatCharge < 100) {
    LCD.print(43, 31, 1, inBatCharge);
  }
  else  {
    LCD.print(49, 31, 1, inBatCharge);
  }
  LCD.print(55, 31, 1, "%");

  if (exBatCharge >= 100) {
    LCD.print(72, 31, 1, exBatCharge);
  }
  else if (exBatCharge >= 10 && exBatCharge < 100) {
    LCD.print(78, 31, 1, exBatCharge);
  }
  else {
    LCD.print(84, 31, 1, exBatCharge);
  }
  LCD.print(91, 31, 1, "%");

  // ток
  LCD.print(0, 41, 1, "Cur");


  if (inBatCurent >= 0) {
    if (inBatCurent  >= 1000) {
      LCD.print(28, 41, 1, (inBatCurent % 10000) / 1000);
    }
    else {
      LCD.print(28, 41, 1, " ");
    }
  }
  else {
    LCD.print(28, 41, 1, "-");
  }

  LCD.print(34, 41, 1, (abs(inBatCurent) % 1000) / 100);
  LCD.print(39, 41, 1, ",");
  LCD.print(43, 41, 1, (abs(inBatCurent) % 100) / 10);
  LCD.print(49, 41, 1, abs(inBatCurent) % 10);
  LCD.print(55, 41, 1, "A");



  if (inBatCurent >= 0) {
    if (exBatCurent  >= 1000) {
      LCD.print(64, 41, 1, (exBatCurent % 10000) / 1000);
    }
    else {
      LCD.print(64, 41, 1, " ");
    }
  }
  else {
    LCD.print(64, 41, 1, "-");
  }
  LCD.print(70, 41, 1, (abs(exBatCurent) % 1000) / 100);
  LCD.print(75, 41, 1, ",");
  LCD.print(79, 41, 1, (abs(exBatCurent) % 100) / 10);
  LCD.print(85, 41, 1, abs(exBatCurent) % 10);
  LCD.print(91, 41, 1, "A");




  // напряжение
  LCD.print(0, 51, 1, "Volt");

  LCD.print(28, 51, 1, inBatVoltage / 100);
  LCD.print(39, 51, 1, ",");  
  LCD.print(43, 51, 1, inBatVoltage % 100);
  LCD.print(55, 51, 1, "V");
  LCD.print(64, 51, 1, exBatVoltage /100);  
  LCD.print(75, 51, 1, ",");
  LCD.print(79, 51, 1, exBatVoltage % 100);  
  LCD.print(91, 51, 1, "V");



  // температура

  LCD.print(0, 61, 1, "Temp");
  if (inBatTemp >= 10) {
    LCD.print(43, 61, 1, inBatTemp);
  }
  else if (inBatTemp >= 0 && inBatTemp < 10) {
    LCD.print(49, 61, 1, inBatTemp);
  }
  else if (inBatTemp > -10 && inBatTemp < 0 ) {
    LCD.print(43, 61, 1, inBatTemp);
  }
  else  {
    LCD.print(36, 61, 1, inBatTemp);
  }
  LCD.drawCircle(57, 62, 1, 1);



  if (exBatTemp >= 10) {
    LCD.print(79, 61, 1, exBatTemp);
  }
  else if (exBatTemp >= 0 && exBatTemp < 10) {
    LCD.print(85, 61, 1, exBatTemp);
  }
  else if (exBatTemp > -10 && exBatTemp < 0 ) {
    LCD.print(79, 61, 1, exBatTemp);
  }
  else  {
    LCD.print(63, 61, 1, exBatTemp);
  }
  LCD.drawCircle(93, 62, 1, 1);





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
