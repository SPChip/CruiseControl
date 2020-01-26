//SPChip
#include "lcd1202.h"
#include "GyverButton.h"  // Библиотека для кнопки

#define BTN_PIN1 10     // кнопка1 подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define NINEBOT_PORT Serial1   // Порт подключения к самокату
#define ESC 0x20               // Контролер
#define BLE 0x21               // Приборка с блютус
#define BMS 0x22               // Контролер батареи
#define IOT 0x3D               // Приложение ? Интернет вещей?
#define APP 0x3E               // Приложение
#define TIMEOUT_QUERY 25        // пауза между запросами, мс 

GButton btn1(BTN_PIN1, HIGH_PULL, NORM_OPEN); // настраиваем кнопки
LCD1202 LCD(8, 7, 6, 5);  // RST, CS, MOSI, SCK
extern uint8_t SmallFont[]; //объявление внешнего массива символов
extern uint8_t MediumNumbers[]; //объявление внешнего массива символов
extern uint8_t BigNumbers[]; //объявление внешнего массива символов

const byte REQ_SPEED[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0x26, 0x02, 0x77, 0xFF}; // запрос скорости
const byte REQ_CURRENT[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0x53, 0x02, 0x4A, 0xFF}; // запрос тока фазы (регистр 53 ESC)
const byte REQ_POWER[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0xBD, 0x02, 0xE0, 0xFE}; // запрос мощности (регистр BD ESC)

const byte REQ_TRIP[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0xB4, 0x10, 0xDB, 0xFE}; // запрос заряд %, скорость, ср. скорость, общий пробег, пробег за поездку, время с включения, температура контроллера

const byte REQ_INBAT[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x31, 0x0A, 0x62, 0xFF}; // запрос к BMS  емкость, %, ток, напряжение, темп
const byte REQ_INCELL[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x40, 0x14, 0x49, 0xFF}; // запрос к BMS банки внутренней батареи


byte curs = 0;                                                      // текущий элемент массива данных, количество данных
byte data[200] = {0};                                               // массив для данных, нулевой элемент = 0
byte dysplayMode = 1;
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

byte contrast = 58;

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
    contrast++;                               // переходим к следующему режиму

  }
  ReceivingData();
  if (millis() < 100) {
    DisplayLogo();
  }
  else if (millis() - lcdtime > 1000) {
    if (newDataFlag) {
      Display1();
      newDataFlag = 0;
      lcdtime = millis();
    }
  }
  timeloop = micros() - timeloop;
  // Serial.println(timeloop);
}

void Display1() {
  int _batCharge = 8;

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
  LCD.print(2, 61, 1, "км");
  
  //время в пути
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

  LCD.print(49, 52, 1, "Time");
  //LCD.print(49, 61, 1, "00:00:00");


  /*




    // секундомер
    int XRidingTime = 0;
    int YRidingTime = 40;
    LCD.printNumF(int(millis() / 3600000), 0, XRidingTime, YRidingTime, ',', 2, '0');
    LCD.print(":", XRidingTime + 12, YRidingTime);
    LCD.printNumF(int((millis() % 3600000) / 60000), 0, XRidingTime + 18, YRidingTime, ',', 2, '0');
    LCD.print(":", XRidingTime + 30, YRidingTime);
    LCD.printNumF( int(((millis() % 3600000) % 60000) / 1000), 0, XRidingTime + 36, YRidingTime, ',', 2, '0');




    LCD.setFont(SmallFont); // Выбор шрифт под названием SmallFont"
    //LCD.print("OOOO", LEFT, 0);
    LCD.print(123, 3, 28);
    LCD.print("22:51", RIGHT, 0);
    //LCD.print("00:00:00", RIGHT, 32);
    LCD.print("00,00km", LEFT, 32);
    LCD.setFont(BigNumbers);
    LCD.printNumI(358, RIGHT, 7);

    LCD.update();
    LCD.clrScr();

    //LCD.printNumI(currentSpeed * 10, CENTER, 0);*/
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



void Unpack() {                 // процедура распаковки пакета

  //PrintPack();
  switch (data[3]) {
    case ESC:
      switch (data[4]) {
        case BLE:
          break;
        case BMS:
          break;
        case APP:
          switch (data[5]) {
            case 0x04:
              switch (data[6]) {
                case 0x26:
                  currentSpeed = (data[8] << 8) | data[7];    // текущая скорость, x10 км/ч
                  //Serial.print(currentSpeed);
                  // Serial.println();
                  newDataFlag = 1;
                  break;


                case 0xB4:
                  // PrintPack();
                  batCharge = (data[8] << 8) | data[7];    // средняя скорость, x10 км/ч
                  averageSpeed = (data[12] << 8) | data[11];    // средняя скорость, x10 км/ч
                  for (int i = 16; i > 12; i--) {               // общий пробег, м
                    totalMileage <<= 8;
                    totalMileage |= data[i];
                  }
                  currentMileage = (data[18] << 8) | data[17];  // текущий пробег, м
                  ridingTime = (data[20] << 8) | data[19];      // время вождения, с
                  escTemp = (data[22] << 8) | data[21];         // температура контроллера, x10 град. цельсия
                  newDataFlag = 1;                              // поднимаем флаг новых данных скорости
                  /*Serial.print(currentSpeed);
                    Serial.print("  ");
                    Serial.print(averageSpeed);
                    Serial.print("  ");
                    Serial.print(totalMileage);
                    Serial.print("  ");
                    Serial.print(currentMileage);
                    Serial.print("  ");
                    Serial.print(ridingTime);
                    Serial.print("  ");
                    Serial.print(escTemp);
                    Serial.println("  ");*/
                  break;
              }
              break;
          }
          break;
      }
      break;
    case BLE:
      switch (data[4]) {
        case ESC:
          //Query(0);
          break;
        case BMS:
          break;
        case APP:
          break;
      }
      break;
    case BMS:
      switch (data[4]) {
        case ESC:
          break;
        case BLE:
          break;
        case APP:
          switch (data[5]) {
            case 0x04:
              switch (data[6]) {
                case 0x31:
                  inBatCapacityLeft = (data[8] << 8) | data[7];    // остаточная емкость внутренней батареи, мА*ч
                  inBatCharge = (data[10] << 8) | data[9];         // заряд внутренней батареи, %
                  inBatCurent = (data[12] << 8) | data[11];        // ток внутренней батареи, x10 мА
                  inBatVoltage = (data[14] << 8) | data[13];       // напряжение внутренней батареи, /100 В
                  newDataFlag = 1;                              // поднимаем флаг новых данных о батарее
                  break;
                case 0x40:
                  byte cellByte = 8;
                  for (int i = 0; i < 10; i++) {
                    inBatCell[i] = (data[cellByte] << 8) | data[cellByte - 1];     // напряжение на банках внутренней батареи /1000 В
                    cellByte += 2;
                    // Serial.print(inBatCell[i]);
                    //Serial.print("  ");
                  }
                  //Serial.println();
                  newDataFlag = 1;                              // поднимаем флаг новых данных о батарее
              }
              break;
          }
          break;
      }
      break;
    case APP:
      switch (data[4]) {
        case ESC:
          break;
        case BLE:
          break;
        case BMS:
          break;
      }
      break;
  }
}


void Query() {                      // процедура отправки запроса
  static unsigned long timerQuery;        // таймер для запроса
  static unsigned long timerReq;
  static byte reqMode;
  static byte reqCounter;
  byte REQ[10];
  if (!passiveMode) {                // если не включен пассивный режим
    if (millis() - timerReq > TIMEOUT_QUERY) {

      if (reqMode > 2) {           // отправляем покругу 3 пакета
        reqMode = 0;
      }
      if (reqCounter > 6) {        // если отправлено 10 пакетов
        switch (dysplayMode) {      // отправляем пакет в соответствии dysplayMode
          case 1:
            reqMode = 3;
            break;
          case 2:
            reqMode = 4;
            break;
          case 3:
            reqMode = 5;
            break;
        }
        reqCounter = 0;
      }
      switch (reqMode) {
        case 0:
          memcpy (REQ, REQ_SPEED, 10);
          break;
        case 1:
          memcpy (REQ, REQ_CURRENT, 10);
          break;
        case 2:
          memcpy (REQ, REQ_POWER, 10);
          break;
        case 3:
          memcpy (REQ, REQ_TRIP, 10);
          break;
        case 4:
          memcpy (REQ, REQ_INBAT, 10);
          break;
        case 5:
          memcpy (REQ, REQ_INCELL, 10);
          break;
      }
      reqMode++;
      reqCounter++;
      timerReq = millis();
      UCSR1B &= ~_BV(RXEN1);            // здесь поменять на UCSR0B &= ~_BV(RXEN0);
      NINEBOT_PORT.write(REQ, 10);
      UCSR1B |= _BV(RXEN1);            // здесь поменять на UCSR0B |= _BV(RXEN0);
    }
  }
}


void ReceivingData() {                                        //процедура получения данных
  static byte readByte;                                                      // переменная для хранения очередного байта из порта
  static byte state = 0;                                                     // переменная состояния 0 - ожидание заголовка, 1- поиск второй части заголовка, 2- заполнения массива данными из буфера, 3- вывод результата
  Query();                                        // отправляем запрос

  while (NINEBOT_PORT.available()) {                               // если в порту есть данные
    readByte = NINEBOT_PORT.read();                                       // читаем из порта байт и он стирается из буфера
    switch (state) {
      case 0:                                                      // если находимся в состоянии 0 поиска заголовка
        if (readByte == 0x5a) {                                           // если принятый байт из буфера = 5А
          state = 1;                                               // включаем состояние 1 для поиска второй части заголовка
          curs = 0;                                                // первый элемент массива
          data[curs++] = readByte; // keep                                // сохраняем в первый элемент массива принятый байт 5A и переходим ко второму элементу (Порядок записи инкремента играет очень большую роль: пост-инкремент var++ возвращает значение переменной var до выполнения этого инкремента. Операция пре-инкремента ++var возвращает значение уже изменённой переменной)
        }
        break;                                                     // выходим из case и возвращемся
      case 1:                                                      // если находимся в состоянии 1
        if (readByte != 0xa5) {                                           // если принятый байт не равен A5
          state = 0;                                               // сбрасываем состояние в 0 - поиск заголовка
        } else {                                                   // иначе (т. е. байт равен второй части заголовка)
          state = 2;                                               // переходим в следующее состояние 2
          data[curs++] = readByte; // keep                                // сохраняем во второй элемент массива принятый байт A5
        }
        break;                                                     // выходим из case и возвращемся
      case 2:                                                      // если находимся в состоянии 2
        if (curs < 2 || (curs >= 2 && curs < (9 + data[2]))) {     // если текущий элемент массива меньше 2 или больше 2 но меньше 9 + третий элемент массива (размер данных из пакета)
          data[curs] = readByte;                                          // сохраняем в массив очередной байт из буфера
          curs++;                                                  // переходим к следующему элементу массива
        } else {                                                   // иначе
          state = 3;                                               // переходим в состояние 3
        }
        break;                                                     // выходим из case и возвращемся в начало
      case 3:                                                      // если находимся в состоянии 3
        unsigned int cs, _cs;
        for (int i = 2; i < curs - 2; i++) {                       // считаем контрольную сумму
          cs = cs + data[i];
        }
        _cs = (data[curs - 1] << 8 ) | data[curs - 2];
        if (_cs == 0xFFFF - cs) {                                  // если контрольная сумма сошлась
          Unpack();                                                // запускаем распаковку
        }
        state = 0;                                                 // обратно в состояние 0 - поиск заголовка
        break;                                                     // выходим из case и возвращемся в начало
    }
  }
}



void PrintPack() {                                                // функция печати пакета
  for (int i = 0; i < curs; i++) {
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}
