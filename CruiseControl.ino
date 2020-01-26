//SPChip
#include "lcd1202.h"
#include "GyverButton.h"  // Библиотека для кнопки

#define BTN_PIN1 10     // кнопка1 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define NINEBOT_PORT Serial1   // Порт подключения к самокату

GButton btn1(BTN_PIN1, HIGH_PULL, NORM_OPEN); // настраиваем кнопки
LCD1202 LCD(8, 7, 6, 5);  // RST, CS, MOSI, SCK

byte curs = 0;                                                      // текущий элемент массива данных, количество данных
byte data[200] = {0};                                               // массив для данных, нулевой элемент = 0
byte dysplayMode = 4;
bool passiveMode = 0;

bool newDataFlag = 1;   // новые данные о скорости (для отрисовки на дисплее)  скорость и др
//bool newDataFlag = 0;   // новые данные о скорости (для отрисовки на дисплее)   батарея
unsigned long timeloop;
unsigned long lcdtime;

unsigned int batCharge;            // % акб
int currentSpeed;       //текущая скорость
int averageSpeed;       //средняя скорость
int maxSpeed;           //максимальная скорость
//int remainingMileage;   //оставшийся пробег
unsigned long totalMileage;      //общий пробег
unsigned long currentMileage;    //текущий пробег
unsigned int ridingTime;         //время вождения
unsigned int escTemp;            // температура контроллера
unsigned int inBatCapacityLeft;  // остаточная емкость внутренней батареи
unsigned int exBatCapacityLeft;  // остаточная емкость внешней батареи
unsigned int inBatCharge;        // заряд внутренней батареи
unsigned int exBatCharge;        // заряд внешней батареи
int inBatCurent;        // ток внутренней батареи
int exBatCurent;        // ток внешней батареи
unsigned int inBatVoltage;       // напряжение внутренней батареи
unsigned int exBatVoltage;       // напряжение  внешней батареи
unsigned int inBatCell[10];      // напряжение на банках внутренней батареи
unsigned int exBatCell[10];      // напряжение на банках внешней батареи


void setup() {
  Serial.begin(115200);                                            // включаем и настраиваем последовательный порт к компу (в мониторе порта поставить тоже 115200)
  NINEBOT_PORT.begin(115200);                                      // включаем и настраиваем последовательный порт к скутеру
  LCD.Inicialize();  //Инициализация дисплея
  LCD.Clear_LCD();  //Очистка дисплея
}

void loop() {
  timeloop = micros();
  btn1.tick();                            // постоянно проверяем первую кнопку
  if (btn1.isClick()) {                   // если кнопка 1 нажата переключаем режим отображения
    //contrast++;                               // переходим к следующему режиму
  }
  ReceivingData();
  if (millis() < 100) {
    DisplayLogo();
  }
  else if (millis() - lcdtime > 1000) {
    if (newDataFlag) {
      Display4();
      newDataFlag = 0;
      lcdtime = millis();
    }
  }
  timeloop = micros() - timeloop;
  // Serial.println(timeloop);
}
