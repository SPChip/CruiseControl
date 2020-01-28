#define ESC 0x20               // Контролер
#define BLE 0x21               // Приборка с блютус
#define BMS 0x22               // Контролер батареи
#define EXBMS 0x23             // Контролер внешней батареи
#define IOT 0x3D               // Приложение ? Интернет вещей?
#define APP 0x3E               // Приложение
#define TIMEOUT_QUERY 25       // пауза между запросами, мс 

const byte REQ_SPEED[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0x26, 0x02, 0x77, 0xFF}; // запрос скорости
const byte REQ_CURRENT[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0x53, 0x02, 0x4A, 0xFF}; // запрос тока фазы (регистр 53 ESC)
const byte REQ_POWER[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0xBD, 0x02, 0xE0, 0xFE}; // запрос мощности (регистр BD ESC)
const byte REQ_TRIP[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0xB4, 0x10, 0xDB, 0xFE}; // запрос заряд %, скорость, ср. скорость, общий пробег, пробег за поездку, время с включения, температура контроллера
const byte REQ_INBAT[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x31, 0x0A, 0x62, 0xFF}; // запрос к внутренней BMS  емкость, %, ток, напряжение, темп
const byte REQ_EXBAT[] = {0x5A, 0xA5, 0x01, 0x3E, 0x23, 0x01, 0x31, 0x0A, 0x61, 0xFF}; // запрос к внешней BMS  емкость, %, ток, напряжение, темп
const byte REQ_INCELL[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x40, 0x14, 0x49, 0xFF}; // запрос к внутренней BMS банки внутренней батареи
const byte REQ_EXCELL[] = {0x5A, 0xA5, 0x01, 0x3E, 0x23, 0x01, 0x40, 0x14, 0x48, 0xFF}; // запрос к внешней BMS банки внутренней батареи

void Query() {                             // процедура отправки запроса
  static unsigned long timerReq;          // таймер для запроса
  static byte reqMode;
  static byte reqCounter;
  static bool reqBat;     // для чередования запрса к внешней и внутренней батарее
  byte REQ[10];
  if (!passiveMode && dysplayMode != 0) {              // если не включен пассивный режим
    if (millis() - timerReq > TIMEOUT_QUERY) {
      if (reqMode > 2) {           // отправляем покругу 3 пакета
        reqMode = 0;
      }
      if (reqCounter > 6) {        // если отправлено 10 пакетов
        switch (dysplayMode) {      // отправляем пакет в соответствии dysplayMode
          case 1:                   // 1 экран данные о поездке + расход
          case 2:                   // 2 экран данные о поездке + ток
          case 3:                   // 3 экран подробные данные о поездке
            reqMode = 3;
            reqCounter = 0;
            break;
          case 4:                   // 4 экран инфо о батареях обоих
            if (reqBat) {
              reqMode = 4;
              reqBat = 0;
            }
            else {
              reqMode = 5;
              reqBat = 1;
            } break;
          case 5:                   // 5 экран банки внутренние
            reqMode = 6;
            reqCounter = 0;
            break;
          case 6:                   // 6 экран банки внешние
            reqMode = 7;
            reqCounter = 0;
            break;
        }
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
          memcpy (REQ, REQ_EXBAT, 10);
          break;
        case 6:
          memcpy (REQ, REQ_INCELL, 10);
          break;
        case 7:
          memcpy (REQ, REQ_EXCELL, 10);
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
                  newDataFlag = 1;
                  break;
                case 0xB4:
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
      PrintPack();
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
                  inBatTemp = max(data[15], data[16]) - 20;           // максимальная из присылаемых температур
                  newDataFlag = 1;                              // поднимаем флаг новых данных о батарее
                  break;
                case 0x40:
                  byte cellByte = 8;
                  for (int i = 0; i < 10; i++) {
                    inBatCell[i] = (data[cellByte] << 8) | data[cellByte - 1];     // напряжение на банках внутренней батареи /1000 В
                    cellByte += 2;
                  }
                  newDataFlag = 1;                              // поднимаем флаг новых данных о батарее
              }
              break;
          }
          break;
      }
      break;
    case EXBMS:
      PrintPack();
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
                  exBatCapacityLeft = (data[8] << 8) | data[7];    // остаточная емкость внутренней батареи, мА*ч
                  exBatCharge = (data[10] << 8) | data[9];         // заряд внутренней батареи, %
                  exBatCurent = (data[12] << 8) | data[11];        // ток внутренней батареи, x10 мА
                  exBatVoltage = (data[14] << 8) | data[13];       // напряжение внутренней батареи, /100 В
                  exBatTemp = max(data[15], data[16]) - 20;           // максимальная из присылаемых температур
                  newDataFlag = 1;                              // поднимаем флаг новых данных о батарее
                  break;
                case 0x40:
                  byte cellByte = 8;
                  for (int i = 0; i < 10; i++) {
                    exBatCell[i] = (data[cellByte] << 8) | data[cellByte - 1];     // напряжение на банках внутренней батареи /1000 В
                    cellByte += 2;
                  }
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
    if (data[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}