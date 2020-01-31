//SPChip
#include "lcd1202.h"      // Библиотека для дисплея
#include "GyverButton.h"  // Библиотека для кнопки
#include <EEPROM.h>       // Библиотека для eeprom
//#include "microDS3231.h"  // Библиотека для часов реального времени

#define BTN_PIN1 10            // кнопка1 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BL_LCD_PIN 4
#define NINEBOT_PORT Serial   // Порт подключения к самокату
#define TIMEOUT_QUERY 50       // пауза между запросами, мс 
#define TIMEOUT_LCD 1000       // пауза между обновлениями экрана

#define INIT_ADDR 1023  // номер резервной ячейки для инициализации первого запуска
#define INIT_KEY 60     // ключ первого запуска. 0-254, на выбор, надо поменять на любое значение и будет как впервый раз
#define DISPLAY_MODE_ADDR 0     // адрес в eeprom для хранения режима экрана dysplayMode

GButton btn1(BTN_PIN1, HIGH_PULL, NORM_OPEN);        // настраиваем кнопку 1
LCD1202 LCD(8, 7, 6, 5);  // RST, CS, MOSI, SCK      // подключаем дисплей
//MicroDS3231 rtc;   // создаем дисплей, подключен к A4, A5

byte curs = 0;                                                      // текущий элемент массива данных, количество данных, потом перенести в ReceivingData
byte data[200] = {0};                                               // массив для принимаемых пакетов, нулевой элемент = 0
byte dysplayMode = 3;                                               // режим экрана
bool passiveMode = 0;                                               // пассивный режим, включается из меню
bool newDataFlag = 0;                                               // новые данные (для отрисовки на дисплее)
bool BL_Lcd = 0;

unsigned long timeloop;                      // для измерения времени выполнения loop
unsigned long lcdTime;                       // таймер для обновления экрана

unsigned int startBatCharge;          // % акб при включении
unsigned int startBatCapacityLeft;    // остаточная емкость обоих батарей при включении
unsigned int batCharge;               // % акб
int currentSpeed;                     //текущая скорость
int averageSpeed;                     //средняя скорость
int maxSpeed;                         //максимальная скорость
unsigned int remainingMileage;        //оставшийся пробег
unsigned long totalMileage;           //общий пробег
unsigned long currentMileage;         //текущий пробег
unsigned int ridingTime;              //время вождения
int escTemp;                          // температура контроллера
unsigned int inBatCapacityLeft;       // остаточная емкость внутренней батареи
unsigned int exBatCapacityLeft;       // остаточная емкость внешней батареи
unsigned int inBatCharge;             // заряд внутренней батареи
unsigned int exBatCharge;             // заряд внешней батареи
int inBatCurent;                      // ток внутренней батареи
int exBatCurent;                      // ток внешней батареи
unsigned int inBatVoltage;            // напряжение внутренней батареи
unsigned int exBatVoltage;            // напряжение  внешней батареи
int inBatTemp;                        // температура внутренней батареи
int exBatTemp;                        // температура  внешней батареи
unsigned int inBatCell[10];           // напряжение на банках внутренней батареи
unsigned int exBatCell[10];           // напряжение на банках внешней батареи


void setup() {
  Serial.begin(115200);          // включаем и настраиваем последовательный порт к компу (в мониторе порта поставить тоже 115200)
  NINEBOT_PORT.begin(115200);    // включаем и настраиваем последовательный порт к скутеру
  pinMode(BL_LCD_PIN, OUTPUT);                         // настраиваем пин на подсветку дисплея
  LCD.Inicialize();              // инициализация дисплея
  LCD.Clear_LCD();               // очистка дисплея
  digitalWrite(BL_LCD_PIN, BL_Lcd);  // включаем/выключаем подсветку дисплея
  DisplayLogo();                 // рисуем лого
  dysplayMode = 3;               // в этом режиме запрашиваются нужные данные для начальных % и емкости батарей
  while (millis() < 5000) {      // ждем 5 сек чтобы наверняка получить все данные
    ReceivingData();             // получаем данные о начальном % и емкости батарей
    if (batCharge > 0 && inBatCapacityLeft > 0 && exBatCapacityLeft > 0) return; // если нужные данные получены выходим из while досрочно
  }
  startBatCharge  = batCharge;   // засекаем начальный заряд батареи
  startBatCapacityLeft = inBatCapacityLeft + exBatCapacityLeft;  // засекаем начальную емкость батарей
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
    EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
    EEPROM.put(DISPLAY_MODE_ADDR, 1);     // режим экрана 1
  }
  EEPROM.get(DISPLAY_MODE_ADDR, dysplayMode);    // читаем режим экрана из eeprom
  
}

void loop() {
  timeloop = micros();
  btn1.tick();                            // постоянно проверяем первую кнопку
  if (btn1.isClick()) {                   // если кнопка 1 нажата переключаем режим отображения
    dysplayMode++;                        // переходим к следующему режиму экрана    
    if (dysplayMode > 6) dysplayMode = 1; // максимум 6 экранов
    EEPROM.put(DISPLAY_MODE_ADDR, dysplayMode);  // записываем в eeprom режим экрана
  }
  ReceivingData();                         // получаем данные
  if (millis() < 100) {
    DisplayLogo();
  }
  else if (millis() - lcdTime > TIMEOUT_LCD) {
    if (newDataFlag) {
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
      newDataFlag = 0;
      lcdTime = millis();
    }
  }
  timeloop = micros() - timeloop;
  //Serial.println(timeloop);


}
