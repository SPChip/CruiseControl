void Settings() {
  byte setting = 1;                             // номер текущей настройки
  byte levelMenu = 1;                           // подпараметры в меню
  bool inputSettings = 0;                       // флаг выбора установки
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
    if (inputSettings == 0 && BTN3.isClick()) {  // если не выбрана установка и нажата кнопка 2
      LCD.Clear_LCD();
      setting++;                                 // переходим к следующей настройке
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
              levelMenu = 1;
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
          break;
        case 3:                                             //пассивный режим
          if (BTN1.isClick()) {
            LCD.Clear_LCD();
            inputSettings = 0;
          }
          if (((BTN2.isClick()) || (BTN3.isClick()))) {
            LCD.Clear_LCD();
            passiveMode = !passiveMode;
            EEPROM.put(PASSIVEMODE_ADDR, passiveMode);  // записываем в eeprom состояние пассивного режима
          }
          break;
        case 4:                                         // настройка PID
          if (BTN1.isClick()) {
            LCD.Clear_LCD();
            levelMenu++;
            if (levelMenu > 3) {
              inputSettings = 0;
              levelMenu = 1;
            }
          }
          switch (levelMenu) {
            case 1:                     //Kp
              if (BTN2.isClick() || BTN2.isStep()) {
                LCD.Clear_LCD();
                float _Kp = REGULATOR.Kp - 0.1;
                if (_Kp < 0) _Kp = 0;
                REGULATOR.Kp = _Kp;
                EEPROM.put(KP_ADDR, REGULATOR.Kp);  // записываем в eeprom значение Kp
              }
              if (BTN3.isClick() || BTN3.isStep()) {
                LCD.Clear_LCD();
                float _Kp = REGULATOR.Kp + 0.1;
                if (_Kp >  9.9) _Kp =  9.9;
                REGULATOR.Kp = _Kp;
                EEPROM.put(KP_ADDR, REGULATOR.Kp);  // записываем в eeprom значение Kp
              }
              break;
            case 2:                     //Ki
              if (BTN2.isClick() || BTN2.isStep()) {
                LCD.Clear_LCD();
                float _Ki = REGULATOR.Ki - 0.1;
                if (_Ki < 0) _Ki = 0;
                REGULATOR.Ki = _Ki;
                EEPROM.put(KI_ADDR, REGULATOR.Ki);  // записываем в eeprom значение Ki
              }
              if (BTN3.isClick() || BTN3.isStep()) {
                LCD.Clear_LCD();
                float _Ki = REGULATOR.Ki + 0.1;
                if (_Ki > 9.9) _Ki = 9.9;
                REGULATOR.Ki = _Ki;
                EEPROM.put(KI_ADDR, REGULATOR.Ki);  // записываем в eeprom значение Ki
              }
              break;
            case 3:                    //Kd
              if (BTN2.isClick() || BTN2.isStep()) {
                LCD.Clear_LCD();
                float _Kd = REGULATOR.Kd - 0.1;
                if (_Kd < 0) _Kd = 0;
                REGULATOR.Kd = _Kd;
                EEPROM.put(KD_ADDR, REGULATOR.Kd);  // записываем в eeprom значение Kd
              }
              if (BTN3.isClick() || BTN3.isStep()) {
                LCD.Clear_LCD();
                float _Kd = REGULATOR.Kd + 0.1;
                if (_Kd > 9.9 ) _Kd = 9.9;
                REGULATOR.Kd = _Kd;
                EEPROM.put(KD_ADDR, REGULATOR.Kd);  // записываем в eeprom значение Kd
              }
              break;
          }


          break;
      }
    }

    // отрисовка
    // заголовок
    LCD.print(20, 0, 1, "Settings");
    LCD.drawFastHLine(0, 8, 96, 1);
    // курсор
    LCD.print(0, setting * 10 + 1, 1, ">");

    //подсветка
    LCD.print(6, 11, 1, "Backlight");
    if (inputSettings && setting == 1) {
      if (BLINK_TIMER.isReady()) {
        if (BL_Lcd) {
          if (flagBlink) {
            LCD.print(85, 11, 1,  "  ");
          }
          else {
            LCD.print(85, 11, 1,  "ON");
          }
        }
        else {
          if (flagBlink) {
            LCD.print(79, 11, 1,  "   ");
          }
          else {
            LCD.print(79, 11, 1,  "OFF");
          }
        }
        flagBlink = !flagBlink;
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


    //установка часов
    LCD.print(6, 21, 1, "Time");
    //часы
    if (inputSettings && setting == 2 && levelMenu == 1 && !BTN2.state() && !BTN3.state()) {
      if (BLINK_TIMER.isReady()) {
        if (flagBlink) {
          LCD.print(48, 21, 1,  "  ");
        }
        else {

          if (RTC.getHours() < 10) {
            LCD.print(54, 21, 1,  RTC.getHours());
          }
          else {
            LCD.print(48, 21, 1,  RTC.getHours());
          }
        }
        flagBlink = !flagBlink;
      }
    }
    else {
      if (RTC.getHours() < 10) {
        LCD.print(54, 21, 1,  RTC.getHours());
      }
      else {
        LCD.print(48, 21, 1,  RTC.getHours());
      }
    }
    LCD.print(60, 21, 1, ":");
    //минуты
    if (inputSettings && setting == 2 && levelMenu == 2 && !BTN2.state() && !BTN3.state()) {
      if (BLINK_TIMER.isReady()) {
        if (flagBlink) {
          LCD.print(66, 21, 1,  "  ");
        }
        else {

          if (RTC.getMinutes() < 10) {
            LCD.print(66, 21, 1,  "0");
            LCD.print(72, 21, 1,  RTC.getMinutes());
          }
          else {
            LCD.print(66, 21, 1,  RTC.getMinutes());
          }
        }
        flagBlink = !flagBlink;
      }
    }
    else {
      if (RTC.getMinutes() < 10) {
        LCD.print(66, 21, 1,  "0");
        LCD.print(72, 21, 1,  RTC.getMinutes());
      }
      else {
        LCD.print(66, 21, 1,  RTC.getMinutes());
      }
    }
    LCD.print(79, 21, 1, ":");
    //секунды
    if (inputSettings && setting == 2 && levelMenu == 3 && !BTN2.state() && !BTN3.state()) {
      if (BLINK_TIMER.isReady()) {
        if (flagBlink) {
          LCD.print(85, 21, 1,  "  ");
        }
        else {

          if (RTC.getSeconds() < 10) {
            LCD.print(85, 21, 1,  "0");
            LCD.print(91, 21, 1,  RTC.getSeconds());
          }
          else {
            LCD.print(85, 21, 1,  RTC.getSeconds());
          }
        }
        flagBlink = !flagBlink;
      }
    }
    else {
      if (RTC.getSeconds() < 10) {
        LCD.print(85, 21, 1,  "0");
        LCD.print(91, 21, 1,  RTC.getSeconds());
      }
      else {
        LCD.print(85, 21, 1,  RTC.getSeconds());
      }
    }
    // пассивный режим
    LCD.print(6, 31, 1, "PassiveMode");
    if (inputSettings && setting == 3) {
      if (BLINK_TIMER.isReady()) {
        if (passiveMode) {
          if (flagBlink) {
            LCD.print(85, 31, 1,  "  ");
          }
          else {
            LCD.print(85, 31, 1,  "ON");
          }
        }
        else {
          if (flagBlink) {
            LCD.print(79, 31, 1,  "   ");
          }
          else {
            LCD.print(79, 31, 1,  "OFF");
          }
        }
        flagBlink = !flagBlink;
      }
    }
    else {
      if (passiveMode) {
        LCD.print(85, 31, 1, "ON");
      }
      else {
        LCD.print(79, 31, 1, "OFF");
      }
    }

    // PID
    LCD.print(6, 41, 1, "PID");
    if (inputSettings && setting == 4 && levelMenu == 1 && !BTN2.state() && !BTN3.state()) {
      if (BLINK_TIMER.isReady()) {
        if (flagBlink) {
          LCD.print(28, 41, 1,  "   ");
        }
        else {
        LCD.print(28, 41, 1, (int) REGULATOR.Kp);
      LCD.print(34, 41, 1, ".");
      LCD.print(39, 41, 1, (int) (REGULATOR.Kp * 10) % 10);
        }
        flagBlink = !flagBlink;
      }
    }
    else {
      LCD.print(28, 41, 1, (int) REGULATOR.Kp);
      LCD.print(34, 41, 1, ".");
      LCD.print(39, 41, 1, (int) (REGULATOR.Kp * 10) % 10);
    }


    if (inputSettings && setting == 4 && levelMenu == 2 && !BTN2.state() && !BTN3.state()) {
      if (BLINK_TIMER.isReady()) {
        if (flagBlink) {
          LCD.print(54, 41, 1,  "   ");
        }
        else {
           LCD.print(54, 41, 1, (int) REGULATOR.Ki);
          LCD.print(60, 41, 1, ".");
          LCD.print(65, 41, 1, (int) (REGULATOR.Ki * 10) % 10);
        }
        flagBlink = !flagBlink;
      }
    }
    else {
       LCD.print(54, 41, 1, (int) REGULATOR.Ki);
          LCD.print(60, 41, 1, ".");
          LCD.print(65, 41, 1, (int) (REGULATOR.Ki * 10) % 10);
    }



    if (inputSettings && setting == 4 && levelMenu == 3 && !BTN2.state() && !BTN3.state()) {
      if (BLINK_TIMER.isReady()) {
        if (flagBlink) {
          LCD.print(80, 41, 1,  "   ");
        }
        else {
          LCD.print(80, 41, 1, (int) REGULATOR.Kd);
          LCD.print(86, 41, 1, ".");
          LCD.print(91, 41, 1, (int) (REGULATOR.Kd * 10) % 10);
        }
        flagBlink = !flagBlink;
      }
    }
    else {
      LCD.print(80, 41, 1, (int) REGULATOR.Kd);
      LCD.print(86, 41, 1, ".");
      LCD.print(91, 41, 1, (int) (REGULATOR.Kd * 10) % 10);
    }


































    LCD.Update();
  }

}
