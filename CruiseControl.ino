//SPChip
#include "lcd1202.h"
#include "GyverButton.h"  // Библиотека для кнопки
#include <EEPROM.h>       // Библиотека для eeprom
#include "microDS3231.h"  // Библиотека для часов реального времени

#define BTN_PIN1 14     // кнопка1 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_PIN2 15     // кнопка2 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_PIN3 16     // кнопка3 подключена сюда (BTN_PIN --- КНОПКА --- GND)
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

unsigned int startBatCharge;          // % акб при включении
unsigned int startBatCapacityLeft;    // остаточная емкость обоих батарей при включении
unsigned int batCharge;            // % акб
int currentSpeed;       //текущая скорость
int averageSpeed;       //средняя скорость
int maxSpeed;           //максимальная скорость
unsigned int remainingMileage;        //оставшийся пробег
unsigned long totalMileage;      //общий пробег
unsigned long currentMileage;    //текущий пробег
unsigned int ridingTime;         //время вождения
int escTemp;            // температура контроллера
unsigned int inBatCapacityLeft;  // остаточная емкость внутренней батареи
unsigned int exBatCapacityLeft;  // остаточная емкость внешней батареи
unsigned int inBatCharge;        // заряд внутренней батареи
unsigned int exBatCharge;        // заряд внешней батареи
int inBatCurent;        // ток внутренней батареи
int exBatCurent;        // ток внешней батареи
unsigned int inBatVoltage;       // напряжение внутренней батареи
unsigned int exBatVoltage;       // напряжение  внешней батареи
int inBatTemp;                   // напряжениетемпература внутренней батареи
int exBatTemp;                   // температура  внешней батареи
unsigned int inBatCell[10];      // напряжение на банках внутренней батареи
unsigned int exBatCell[10];      // напряжение на банках внешней батареи


void setup() {
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
  }
  EEPROM.get(DISPLAY_MODE_ADDR, dysplayMode);    // читаем режим экрана из eeprom
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
    }
    lcdtime = millis();
  }
}
