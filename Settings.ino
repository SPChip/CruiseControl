void Settings() {
  byte setting = 1;                             // номер текущей настройки
  byte levelMenu = 1;                           // подпараметры в меню
  bool inputSettings = 0;                       // флаг выбора установки
  unsigned long timerBlink;                     // таймер для мигания
  unsigned long timerBlinkCursor; 
  bool flagBlink = 1;                           // флаг для мигания
   
  LCD.Clear_LCD();                              // очищаем дисплей
  while (!BTN1.isHolded()) {                    // пока не будет удержана кнопка 1
    BTN1.tick();                                // постоянно проверяем все три кнопки
    BTN2.tick();
    BTN3.tick();
    if (inputSettings == 0 && BTN2.isClick()) {  // если не выбрана установка и нажата кнопка 2
      LCD.Clear_LCD();
      setting--;                                 // переходим к предъидущей настройке
      if (setting < 1) {                         // максимум 6 настроек
        setting = 6;
      }
    }
    if (inputSettings == 0 && BTN3.isClick()) {
      LCD.Clear_LCD();
      setting++;
      if (setting > 6) {
        setting = 1;
      }
    }
    if (inputSettings == 0 && BTN1.isClick()) {  // если не выбрана установка и нажата кнопка 1
      LCD.Clear_LCD();
      inputSettings = 1;                          // входим в установку
    }
    if (inputSettings) {                           // если вошли в установку
      switch (setting) {                           // установка
        case 1:                                             //подсветка
          if (BTN1.isClick()) {
            LCD.Clear_LCD();
            inputSettings = 0;
          }
          if (((BTN2.isClick()) || (BTN3.isClick()))) {
            LCD.Clear_LCD();
            BL_Lcd = !BL_Lcd;
            digitalWrite(BL_LCD_PIN, BL_Lcd);
            EEPROM.put(BACKLIGHT_ADDR, BL_Lcd);  // записываем в eeprom режим подсветки
          }
          break;
        case 2:
          if (BTN1.isClick()) {
            LCD.Clear_LCD();
            levelMenu++;
            if (levelMenu > 3) {
              inputSettings = 0;
            }
          }
          switch (levelMenu) {
            case 1:                     //установка часов
              if (BTN2.isClick() || BTN2.isStep()) {
                LCD.Clear_LCD();
                char hour = RTC.getHours() - 1;
                if (hour < 0) hour = 23;
                RTC.setTime(RTC.getSeconds(), RTC.getMinutes(), hour, RTC.getDate(), RTC.getMonth(), RTC.getYear());
              }
              if (BTN3.isClick() || BTN3.isStep()) {
                LCD.Clear_LCD();
                char hour = RTC.getHours() + 1;
                if (hour >= 24) hour = 0;
                RTC.setTime(RTC.getSeconds(), RTC.getMinutes(), hour, RTC.getDate(), RTC.getMonth(), RTC.getYear());
              }
              break;
            case 2:                     //установка минут
              if (BTN2.isClick() || BTN2.isStep()) {
                LCD.Clear_LCD();
                char minutes = RTC.getMinutes() - 1;
                if (minutes < 0) minutes = 59;
                RTC.setTime(RTC.getSeconds(), minutes, RTC.getHours(), RTC.getDate(), RTC.getMonth(), RTC.getYear());
              }
              if (BTN3.isClick() || BTN3.isStep()) {
                LCD.Clear_LCD();
                char minutes = RTC.getMinutes() + 1;
                if (minutes >= 60) minutes = 0;
                RTC.setTime(RTC.getSeconds(), minutes, RTC.getHours(), RTC.getDate(), RTC.getMonth(), RTC.getYear());
              }
              break;
            case 3:                     //установка секунд
              if (BTN2.isClick() || BTN2.isStep()) {
                LCD.Clear_LCD();
                char sec = RTC.getSeconds() - 1;
                if (sec < 0) sec = 59;
                RTC.setTime(sec, RTC.getMinutes(), RTC.getHours(), RTC.getDate(), RTC.getMonth(), RTC.getYear());
              }
              if (BTN3.isClick() || BTN3.isStep()) {
                LCD.Clear_LCD();
                char sec = RTC.getSeconds() + 1;
                if (sec >= 60) sec = 0;
                RTC.setTime(sec, RTC.getMinutes(), RTC.getHours(), RTC.getDate(), RTC.getMonth(), RTC.getYear());
              }
              break;

          }

      }
    }

    // отрисовка
    //заголовок
    LCD.print(20, 0, 1, "Settings");
    LCD.drawFastHLine(0, 8, 96, 1);
    // курсор
    //if (inputSettings == 0) {
      LCD.print(0, setting * 10 + 1, 1, ">");
    /*}
    else {
      if (millis() - timerBlinkCursor > 500 ) {
        if (flagBlink) {
          LCD.print(0, setting * 10 + 1, 1, " ");
        }
        else {
          LCD.print(0, setting * 10 + 1, 1, ">");
        }
        timerBlinkCursor = millis();
       
      }
    }*/

    //подсветка
    LCD.print(6, 11, 1, "Backlight");

    if (inputSettings == 0) {
      if (BL_Lcd) {
        LCD.print(85, 11, 1, "ON");
      }
      else {
        LCD.print(79, 11, 1, "OFF");
      }
    }
    else if (setting == 1) {
      if (BL_Lcd) {
        if (millis() - timerBlink > 500 ) {
          if (flagBlink) {
            LCD.print(85, 11, 1, "  ");
          }
          else {
            LCD.print(85, 11, 1, "ON");
          }
          timerBlink = millis(); 
          flagBlink = !flagBlink;
          
        }
      }
      else{
        if (millis() - timerBlink > 500 ) {
          if (flagBlink) {
            LCD.print(79, 11, 1, "   ");
          }
          else {
            LCD.print(79, 11, 1, "OFF");
          }
          timerBlink = millis(); 
          flagBlink = !flagBlink;
          
        }
      }
    }
    else {
      if (BL_Lcd) {
        LCD.print(85, 11, 1, "ON");
      }
      else {
        LCD.print(79, 11, 1, "OFF");
      }
    }
    



    //часы
    LCD.print(6, 21, 1, "Time");
    LCD.print(49, 21, 1, RTC.getHours());
    LCD.print(61, 21, 1, ":");
    LCD.print(67, 21, 1, RTC.getMinutes());
    LCD.print(79, 21, 1, ":");
    LCD.print(85, 21, 1, RTC.getSeconds());





    LCD.Update();
  }

}
