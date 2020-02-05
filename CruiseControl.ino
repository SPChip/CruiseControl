//SPChip 1.0
#include "lcd1202.h"      // Библиотека для дисплея
#include "GyverButton.h"  // Библиотека для кнопок
#include "GyverTimer.h"   // Библиотека для таймеров
#include "GyverPID.h"     // Библиотека для PID регулятора
#include <EEPROM.h>       // Библиотека для eeprom
<<<<<<< HEAD
#include "microDS3231.h"  // Библиотека для часов реального времени
=======
#include "microDS3231.h"  // Библиотека для часов
>>>>>>> master

#define BTN_PIN1 14     // кнопка1 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_PIN2 15     // кнопка2 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_PIN3 16     // кнопка3 подключена сюда (BTN_PIN --- КНОПКА --- GND)
<<<<<<< HEAD
#define BL_LCD_PIN 4    // подсветка дисплея подключена сюда
#define NINEBOT_PORT Serial1   // Порт подключения к самокату
#define TIMEOUT_QUERY 50       // пауза между запросами, мс 
#define TIMEOUT_LCD 900       // пауза между обновлениями экрана, мс 


#define INIT_ADDR 1023  // номер резервной ячейки для инициализации первого запуска
#define INIT_KEY 70     // ключ первого запуска. 0-254, на выбор, надо поменять на любое значение и будет как впервый раз
#define DISPLAY_MODE_ADDR 0     // адрес в eeprom для хранения режима экрана dysplayMode
#define BACKLIGHT_ADDR 10     // адрес в eeprom для хранения режима подсветки дисплея


GButton BTN1(BTN_PIN1, HIGH_PULL, NORM_OPEN); // настраиваем кнопку 1
GButton BTN2(BTN_PIN2, HIGH_PULL, NORM_OPEN); // настраиваем кнопку 2
GButton BTN3(BTN_PIN3, HIGH_PULL, NORM_OPEN); // настраиваем кнопку 3
LCD1202 LCD(8, 7, 6, 5);  // RST, CS, MOSI, SCK
MicroDS3231 RTC;

byte curs = 0;                                                      // текущий элемент массива данных, количество данных
byte data[200] = {0};                                               // массив для данных, нулевой элемент = 0
byte dysplayMode = 1;
bool passiveMode = 0;
bool newDataFlag = 0;   // новые данные о скорости (для отрисовки на дисплее)  скорость и др
bool BL_Lcd = 0;


unsigned long lcdtime;
=======
#define BRAKE_PIN A0    // ручка тормоза подключена сюда, потом диод и потом в BLE
#define BL_LCD_PIN 4           // подсветка дисплея подключена сюда
#define NINEBOT_PORT Serial1   // Порт подключения к самокату
#define TIMEOUT_QUERY 50       // пауза между запросами, мс 
#define TIMEOUT_LCD 900        // пауза между обновлениями экрана, мс 


#define INIT_ADDR 1023        // номер резервной ячейки для инициализации первого запуска
#define INIT_KEY 44           // ключ первого запуска. 0-254, на выбор, надо поменять на любое значение и будет как впервый раз
#define DISPLAY_MODE_ADDR 0   // адрес в eeprom для хранения режима экрана dysplayMode
#define BACKLIGHT_ADDR 10     // адрес в eeprom для хранения режима подсветки дисплея
#define PASSIVEMODE_ADDR 20   // адрес в eeprom для хранения состояния пассивного режима
#define KP_ADDR 30            // адрес в eeprom для хранения коэффициента Kp
#define KI_ADDR 40            // адрес в eeprom для хранения коэффициента Ki
#define KD_ADDR 50            // адрес в eeprom для хранения коэффициента Kd
#define STEP_ADDR 60          // адрес в eeprom для хранения шага изменения скорости


GButton BTN1(BTN_PIN1, HIGH_PULL, NORM_OPEN);     // настраиваем кнопку 1
GButton BTN2(BTN_PIN2, HIGH_PULL, NORM_OPEN);     // настраиваем кнопку 2
GButton BTN3(BTN_PIN3, HIGH_PULL, NORM_OPEN);     // настраиваем кнопку 3
LCD1202 LCD(8, 7, 6, 5);  // RST, CS, MOSI, SCK   // подключаем дисплей
MicroDS3231 RTC;                                  // подключаем часы реального времени
GTimer BLINK_TIMER(MS, 500);                      // создаем таймер для мигания в меню настроек
GTimer LCD_TIMER(MS, TIMEOUT_LCD);                // создаем таймер для отрисовки дисплея
GyverPID REGULATOR(0, 0, 0, 100);                 // создаем регулятор PID

byte curs = 0;                       // текущий элемент массива данных, количество данных
byte data[200] = {0};                // массив для данных, нулевой элемент = 0
byte dysplayMode;                    // режим экрана
bool passiveMode;                    // флаг пассивного режима
bool newDataFlag = 0;                // новые данные о скорости (для отрисовки на дисплее)  скорость и др
bool BL_Lcd;                         // состояние подсветки вкл/выкл
bool cruiseControlFlag = 0;          // флаг включения круиз контроля

>>>>>>> master

unsigned int startBatCharge;          // % акб при включении
unsigned int startBatCapacityLeft;    // остаточная емкость обоих батарей при включении
unsigned int batCharge;               // % акб
int currentSpeed;                     // текущая скорость
int presetSpeed;                      // установленная скорость для круиз контроля
int stepChangeSpeed;                  // шаг изменения скорости
int averageSpeed;                     // средняя скорость
int maxSpeed;                         // максимальная скорость
unsigned int remainingMileage;        // оставшийся пробег
unsigned long totalMileage;           // общий пробег
unsigned long currentMileage;         // текущий пробег
unsigned int ridingTime;              // время вождения
int escTemp;                          // температура контроллера
unsigned int inBatCapacityLeft;       // остаточная емкость внутренней батареи
unsigned int exBatCapacityLeft;       // остаточная емкость внешней батареи
unsigned int inBatCharge;             // заряд внутренней батареи
unsigned int exBatCharge;             // заряд внешней батареи
int inBatCurent;                      // ток внутренней батареи
int exBatCurent;                      // ток внешней батареи
unsigned int inBatVoltage;            // напряжение внутренней батареи
unsigned int exBatVoltage;            // напряжение  внешней батареи
int inBatTemp;                        // напряжениетемпература внутренней батареи
int exBatTemp;                        // температура  внешней батареи
unsigned int inBatCell[10];           // напряжение на банках внутренней батареи
unsigned int exBatCell[10];           // напряжение на банках внешней батареи


void setup() {
<<<<<<< HEAD
  Serial.begin(115200);                                            // включаем и настраиваем последовательный порт к компу (в мониторе порта поставить тоже 115200)
  NINEBOT_PORT.begin(115200);                                      // включаем и настраиваем последовательный порт к скутеру
  pinMode(BL_LCD_PIN, OUTPUT);                         // настраиваем пин на подсветку дисплея
  LCD.Inicialize();  //Инициализация дисплея
  LCD.Clear_LCD();  //Очистка дисплея
  digitalWrite(BL_LCD_PIN, BL_Lcd);  // включаем/выключаем подсветку дисплея
  DisplayLogo();                 // рисуем лого
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
    EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
    EEPROM.put(DISPLAY_MODE_ADDR, 1);     // режим экрана 1
    EEPROM.put(BACKLIGHT_ADDR, 0);     // подсветка выключена
=======
  Serial.begin(115200);                     // включаем и настраиваем последовательный порт к компу (в мониторе порта поставить тоже 115200)
  NINEBOT_PORT.begin(115200);               // включаем и настраиваем последовательный порт к скутеру
  pinMode(BL_LCD_PIN, OUTPUT);              // настраиваем пин на подсветку дисплея
  LCD.Inicialize();                         // инициализация дисплея
  LCD.Clear_LCD();                          // очистка дисплея
  DisplayLogo();                            // рисуем лого
  BTN1.setTimeout(2000);                    // таймаут для долгого нажатия кнопки 1
  BTN2.setStepTimeout(200);                 // установка таймаута между инкрементами ( с какой скоростью будут меняться цифры при удержании кнопки)
  BTN3.setStepTimeout(200);                 // установка таймаута между инкрементами ( с какой скоростью будут меняться цифры при удержании кнопки)
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
    EEPROM.write(INIT_ADDR, INIT_KEY);      // записали ключ
    EEPROM.put(DISPLAY_MODE_ADDR, 1);       // режим экрана 1
    EEPROM.put(BACKLIGHT_ADDR, 0);          // подсветка выключена
    EEPROM.put(PASSIVEMODE_ADDR, 0);        // пассивный режим выключен
    EEPROM.put(KP_ADDR, 1.0);               // начальное значение Kp
    EEPROM.put(KI_ADDR, 1.0);               // начальное значение Ki
    EEPROM.put(KD_ADDR, 1.0);               // начальное значение Kd
    EEPROM.put(STEP_ADDR, 10);              // начальное значение шага изменения скорости
>>>>>>> master
  }
  EEPROM.get(BACKLIGHT_ADDR, BL_Lcd);            // читаем режим подсветки
  digitalWrite(BL_LCD_PIN, BL_Lcd);              // включаем/выключаем подсветку дисплея
  EEPROM.get(DISPLAY_MODE_ADDR, dysplayMode);    // читаем режим экрана из eeprom
<<<<<<< HEAD
  EEPROM.get(BACKLIGHT_ADDR, BL_Lcd);    // читаем режим подсветки
  //RTC.setTime(30, 23, 11, 04, 02, 2020); // устанвока времени вручную
  digitalWrite(BL_LCD_PIN, BL_Lcd);  // включаем/выключаем подсветку дисплея
  BTN1.setTimeout(2000);           // таймаут для долгого нажатия кнопки 1
  BTN2.setStepTimeout(200);                    // установка таймаута между инкрементами ( с какой скоростью будут меняться цифры при удержании кнопки)
  BTN3.setStepTimeout(200);
}

void loop() {
 BTN1.tick();                            // постоянно проверяем первую кнопку
  if (BTN1.isClick()) {                   // если кнопка 1 нажата переключаем режим отображения
    dysplayMode++;                        // переходим к следующему режиму экрана
    if (dysplayMode > 6) dysplayMode = 1; // максимум 6 экранов
    EEPROM.put(DISPLAY_MODE_ADDR, dysplayMode);  // записываем в eeprom режим экрана
  }
  if (BTN1.isHolded()) {                  // если кнопка 1 зажата входим в режим установки часов
    Settings();
  }
  ReceivingData();
  if (millis() - lcdtime > TIMEOUT_LCD) {
    switch (dysplayMode) {
      case 0:
        break;
      case 1:
        Display1();
        break;
      case 2:
        break;
      case 3:
        Display3();
        break;
      case 4:
        Display4();
        break;
      case 5:
        Display5();
        break;
      case 6:
        Display6();
        break;
=======
  EEPROM.get(PASSIVEMODE_ADDR, passiveMode);     // читаем состояние пассивного режима
  EEPROM.get(KP_ADDR, REGULATOR.Kp);             // читаем и устанавливаем значение Kp
  EEPROM.get(KI_ADDR, REGULATOR.Ki);             // читаем и устанавливаем значение Ki
  EEPROM.get(KD_ADDR, REGULATOR.Kd);             // читаем и устанавливаем значение Kd
  EEPROM.get(STEP_ADDR, stepChangeSpeed);        // читаем и устанавливаем значение шага изменения скорости
}



void loop() {
  BTN1.tick();                                   // постоянно проверяем первую кнопку
  BTN2.tick();                                   // постоянно проверяем вторую кнопку
  BTN3.tick();                                   // постоянно проверяем третью кнопку
  if (BTN1.isClick()) {                          // если кнопка 1 нажата переключаем режим отображения
    dysplayMode++;                               // переходим к следующему режиму экрана
    if (dysplayMode > 6) dysplayMode = 1;        // максимум 6 экранов
    EEPROM.put(DISPLAY_MODE_ADDR, dysplayMode);  // записываем в eeprom режим экрана
  }
  if (BTN1.isHolded()) {                         // если кнопка 1 зажата входим в меню установок
    Settings();
  }
  if (BTN2.isClick()|| BTN2.isStep()) {                          // если кнопка 2 нажата
    if (!cruiseControlFlag) {                    // если круиз контроль выключен
      cruiseControlFlag  = 1;                    // включаем круиз контроль
      presetSpeed = currentSpeed;                // запоминаем текущую скорость
    }
    else {                                       // если круиз контроль включен
      presetSpeed = presetSpeed - stepChangeSpeed; // увеличиваем установленную скорость на заданный шаг
>>>>>>> master
    }
  }
<<<<<<< HEAD
=======
  if (BTN3.isClick()|| BTN3.isStep()) {                          // если кнопка 3 нажата
    if (!cruiseControlFlag) {                    // если круиз контроль выключен
      cruiseControlFlag  = 1;                    // включаем круиз контроль
      presetSpeed = currentSpeed;                // запоминаем текущую скорость
    }
    else {                                       // если круиз контроль включен
      presetSpeed = presetSpeed + stepChangeSpeed; // увеличиваем установленную скорость на заданный шаг
    }
  }
  if (analogRead(BRAKE_PIN) > 200) {             // постоянно проверяем ручку томоза и если она нажата на ...%
    cruiseControlFlag = 0;                       // выключаем круиз контроль
  }
  if (cruiseControlFlag) {                       // если флаг круиза поднят
    CruiseControl();                             // вызываем функцию круиза
  }
  ReceivingData();                               // постоянно обмениваемся данными
  Display();                                     // постоянно вызываем функцию отрисовки экрана
>>>>>>> master
}
