#define NINEBOT_PORT Serial1   // Порт подключения к самокату
#define ESC 0x20               // Контролер
#define BLE 0x21               // Приборка с блютус
#define BMS 0x22               // Контролер батареи
#define IOT 0x3D               // Приложение ? Интернет вещей?
#define APP 0x3E               // Приложение
#define timeoutQuery 100       // пауза между запросами, мс                                     

byte c;                                                             // переменная для хранения очередного байта из порта
byte state = 0;                                                     // переменная состояния 0 - ожидание заголовка, 1- поиск второй части заголовка, 2- заполнения массива данными из буфера, 3- вывод результата
byte curs = 0;                                                      // текущий элемент массива данных, количество данных
byte data[200] = {0};                                               // массив для данных, нулевой элемент = 0 (100 много, вроде должно быть не более 64)
const byte req0[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0xB0, 0x20, 0xCF, 0xFE};    //indexQuery 0 скорость, время, пробег и т.д.
const byte req1[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x31, 0x0A, 0x62, 0xFF};    //indexQuery 1 батарея: напряж ток % емк
const byte req2[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x40, 0x10, 0x4D, 0xFF};    //indexQuery 2 батарея: первые 8 банок
const byte req3[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x48, 0x10, 0x45, 0xFF};    //indexQuery 3 батарея: последние 2 банки
long timerQuery;        // таймер для запроса
bool newDataFlag = 0;   // новые данные о скорости (для отрисовки на дисплее)

int currentSpeed;       //текущая скорость
int averageSpeed;       //средняя скорость
int maxSpeed;           //максимальная скорость
//int remainingMileage;   //оставшийся пробег
long totalMileage;      //общий пробег
long currentMileage;    //текущий пробег
int ridingTime;         //время вождения
int escTemp;            // температура контроллера
int inBatCharge;        // заряд внутренней батареи
int exBatCharge;        // заряд внешней батареи
int inBatCapacityLeft;  // остаточная емкость внутренней батареи
int exBatCapacityLeft;  // остаточная емкость внешней батареи
int inBatCurent;        // ток внутренней батареи
int exBatCurent;        // ток внешней батареи
int inBatVoltage;       // напряжение внутренней батареи
int exBatVoltage;       // напряжение  внешней батареи
int inBatCell[10];      // напряжение на банках внутренней батареи
int exBatCell[10];      // напряжение на банках внешней батареи

void setup() {
  Serial.begin(115200);                                            // включаем и настраиваем последовательный порт к компу (в мониторе порта поставить тоже 115200)
  NINEBOT_PORT.begin(115200);                                           // включаем и настраиваем последовательный порт к скутеру
}

void loop() {
  ReceivingData();
}


void Unpack() {                 // процедура распаковки пакета
  //PrintPack();                //выводим пакет
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
                case 0xB0:
                  currentSpeed = (data[18] << 8) | data[17];    // текущая скорость, x10 км/ч
                  averageSpeed = (data[20] << 8) | data[19];    // средняя скорость, x10 км/ч
                  totalMileage = data[24] << 24 | data[23] << 16 | data[22] << 8 | data[21];  // общий пробег, м
                  currentMileage = (data[26] << 8) | data[25];  // текущий пробег, м
                  ridingTime = (data[28] << 8) | data[27];      // время вождения, с
                  escTemp = (data[30] << 8) | data[29];         // температура контроллера, x10 град. цельсия
                  if (currentSpeed > maxSpeed) {                // вычисляем максимальную скорость
                    maxSpeed = currentSpeed;
                  }
                  newDataFlag = 1;                              // поднимаем флаг новых данных
                  PrintPack();                                  // выводим пакет
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


void Query(byte indexQuery) {        //процедура отправки запрса
  UCSR1B &= ~_BV(RXEN1);
  switch (indexQuery) {
    case 0:
      NINEBOT_PORT.write(req0, sizeof(req0));
      break;
    case 1:
      NINEBOT_PORT.write(req1, sizeof(req0));
      break;
    case 2:
      NINEBOT_PORT.write(req2, sizeof(req0));
      break;
    case 3:
      NINEBOT_PORT.write(req3, sizeof(req0));
      break;
  }
  UCSR1B |= _BV(RXEN1);
}


void ReceivingData() {                                        //процедура получения данных
  static byte indexQuery;
  if (millis() - timerQuery > timeoutQuery ) {    
    Query(indexQuery);                                        // отправляем 4 запроса по кругу
    indexQuery++;
    if (indexQuery>3) {indexQuery=0;}
    timerQuery = millis();
  }
  if (NINEBOT_PORT.available()) {                                  // если в порту есть данные
    c = NINEBOT_PORT.read();                                       // читаем из порта байт и он стирается из буфера
    switch (state) {
      case 0:                                                      // если находимся в состоянии 0 поиска заголовка
        if (c == 0x5a) {                                           // если принятый байт из буфера = 5А
          state = 1;                                               // включаем состояние 1 для поиска второй части заголовка
          curs = 0;                                                // первый элемент массива
          data[curs++] = c; // keep                                // сохраняем в первый элемент массива принятый байт 5A и переходим ко второму элементу (Порядок записи инкремента играет очень большую роль: пост-инкремент var++ возвращает значение переменной var до выполнения этого инкремента. Операция пре-инкремента ++var возвращает значение уже изменённой переменной)
        }
        break;                                                     // выходим из case и возвращемся
      case 1:                                                      // если находимся в состоянии 1
        if (c != 0xa5) {                                           // если принятый байт не равен A5
          state = 0;                                               // сбрасываем состояние в 0 - поиск заголовка
        } else {                                                   // иначе (т. е. байт равен второй части заголовка)
          state = 2;                                               // переходим в следующее состояние 2
          data[curs++] = c; // keep                                // сохраняем во второй элемент массива принятый байт A5
        }
        break;                                                     // выходим из case и возвращемся
      case 2:                                                      // если находимся в состоянии 2
        if (curs < 2 || (curs >= 2 && curs < (9 + data[2]))) {     // если текущий элемент массива меньше 2 или больше 2 но меньше 9 + третий элемент массива (размер данных из пакета)
          data[curs] = c;                                          // сохраняем в массив очередной байт из буфера
          curs++;                                                  // переходим к следующему элементу массива
        } else {                                                   // иначе
          state = 3;                                               // переходим в состояние 3
        }
        break;                                                     // выходим из case и возвращемся в начало
      case 3:                                                      // если находимся в состоянии 3
        int cs = 0;
        for (int i = 2; i < curs - 2; i++) {                          // считаем контрольную сумму
          cs = cs + data[i];
        }
        if ((data[curs - 1] << 8 ) | data[curs - 2] == 0xFFFF - cs) { // если контрольная сумма сошлась
          Unpack();                                                 // запускаем распаковку
        }
        state = 0;                                                 // обратно в состояние 0 - поиск заголовка
        break;                                                     // выходим из case и возвращемся в начало
    }
  }
}

void PrintPack() {
  for (int i = 0; i < curs; i++) {
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}
