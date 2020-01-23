//SPChip
#define NINEBOT_PORT Serial1   // Порт подключения к самокату
#define ESC 0x20               // Контролер
#define BLE 0x21               // Приборка с блютус
#define BMS 0x22               // Контролер батареи
#define IOT 0x3D               // Приложение ? Интернет вещей?
#define APP 0x3E               // Приложение
#define timeoutQuery 25       // пауза между запросами, мс                                     
//new
const unsigned char RECV_TIMEOUT =  5;     // таймаут приема пакета
const unsigned char RECV_BUFLEN  = 64;     // максимальная длина буфера приема пакета
byte AnswerHeader[4];

byte c;                                                             // переменная для хранения очередного байта из порта
byte state = 0;                                                     // переменная состояния 0 - ожидание заголовка, 1- поиск второй части заголовка, 2- заполнения массива данными из буфера, 3- вывод результата
byte curs = 0;                                                      // текущий элемент массива данных, количество данных
byte data[200] = {0};                                               // массив для данных, нулевой элемент = 0 (100 много, вроде должно быть не более 64)

// запросы     преамбула   кол-во от   кому кома-  реги- кол-во контрольная
     //                     байт              нда   стр   рег.     сумма (посчитаем отдельно)
byte req0[] = {0x5A, 0xA5, 0x01, 0x3E, 0x20, 0x01, 0xB0, 0x20, 0x00, 0x00};    //indexQuery 0 скорость, время, пробег и т.д.
//const byte req1[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x31, 0x0A, 0x62, 0xFF};    //indexQuery 1 батарея: напряж ток % емк
//const byte req2[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x40, 0x10, 0x4D, 0xFF};    //indexQuery 2 батарея: первые 8 банок
//const byte req3[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x40, 0x14, 0x49, 0xFF};
//const byte req3[] = {0x5A, 0xA5, 0x01, 0x3E, 0x22, 0x01, 0x48, 0x10, 0x45, 0xFF};    //indexQuery 3 батарея: последние 2 банки
long timerQuery;        // таймер для запроса
bool newDataSpeedFlag = 0;   // новые данные о скорости (для отрисовки на дисплее)  скорость и др
bool newDataBatFlag = 0;   // новые данные о скорости (для отрисовки на дисплее)   батарея
long timeloop;
byte j;

int currentSpeed;       //текущая скорость
int averageSpeed;       //средняя скорость
int maxSpeed;           //максимальная скорость
//int remainingMileage; //оставшийся пробег
long totalMileage;      //общий пробег
long currentMileage;    //текущий пробег
int ridingTime;         //время вождения
int escTemp;            // температура контроллера
int inBatCapacityLeft;  // остаточная емкость внутренней батареи
int exBatCapacityLeft;  // остаточная емкость внешней батареи
int inBatCharge;        // заряд внутренней батареи
int exBatCharge;        // заряд внешней батареи
int inBatCurent;        // ток внутренней батареи
int exBatCurent;        // ток внешней батареи
int inBatVoltage;       // напряжение внутренней батареи
int exBatVoltage;       // напряжение  внешней батареи
int inBatCell[10];      // напряжение на банках внутренней батареи
int exBatCell[10];      // напряжение на банках внешней батареи

void setup() {
  Serial.begin(115200);                                     // включаем и настраиваем последовательный порт к компу (в мониторе порта поставить тоже 115200)
  NINEBOT_PORT.begin(115200);                               // включаем и настраиваем последовательный порт к скутеру
  unsigned int cs = 0xFFFF;
  for (int i = 2; i < 8; i++) {
    cs -= req0[i];
  }
  req0[8] = (byte) cs;
  req0[9] = (byte) (cs>> 8);
}

void loop() {
  static byte indexQuery1;
  if (millis() - timerQuery > timeoutQuery ) {
    Query(indexQuery1);                                        // отправляем 4 запроса по кругу
    indexQuery1++;
    if (indexQuery1 > 3) {
      indexQuery1 = 0;
    }
    timerQuery = millis();
  }


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
                  for (int i = 24; i > 20; i--) {               // общий пробег, м
                    totalMileage <<= 8;
                    totalMileage |= data[i];
                  }
                  currentMileage = (data[26] << 8) | data[25];  // текущий пробег, м
                  ridingTime = (data[28] << 8) | data[27];      // время вождения, с
                  escTemp = (data[30] << 8) | data[29];         // температура контроллера, x10 град. цельсия
                  if (currentSpeed > maxSpeed) {                // вычисляем максимальную скорость
                    maxSpeed = currentSpeed;
                  }
                  newDataSpeedFlag = 1;                              // поднимаем флаг новых данных
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
                    Serial.println("  ");
                  */



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
                  newDataBatFlag = 1;                              // поднимаем флаг новых данных
                  break;

                case 0x40:
                  //PrintPack();
                  j = 8;
                  for (int i = 0; i < 10; i++) {
                    inBatCell[i] = (data[j] << 8) | data[j - 1];     // напряжение на банках внутренней батареи (первые 8 банок) /1000 В
                    j += 2;

                    Serial.print(inBatCell[i]);
                    Serial.print("  ");
                  }
                  newDataBatFlag = 1;                              // поднимаем флаг новых данных
                  Serial.println();
                case 0x48:
                  int j = 8;
                  for (int i = 8; i < 10; i++) {
                    inBatCell[i] = (data[j] << 8) | data[j - 1];     // напряжение на банках внутренней батареи (последние 2 банки) /1000 В
                    j += 2;
                  }
                  newDataBatFlag = 1;                              // поднимаем флаг новых данных
                  break;
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


void Query(byte indexQuery) {        //процедура отправки запрса
  UCSR1B &= ~_BV(RXEN1);
  switch (indexQuery) {
    case 0:
      NINEBOT_PORT.write(req0, sizeof(req0));
      break;
    case 1:
      NINEBOT_PORT.write(req0, sizeof(req0));
      break;
    case 2:
      NINEBOT_PORT.write(req0, sizeof(req0));
      break;
    case 3:
      NINEBOT_PORT.write(req0, sizeof(req0));
      break;
  }
  UCSR1B |= _BV(RXEN1);
}


void ReceivingData() {                                        //процедура получения данных
  static unsigned char   step = 0, _step = 0, entry = 1;                // static используется для создания переменной, которая видна только одной функции. Однако в отличие от локальных переменных, которые создаются и уничтожаются при каждом вызове функции, статические переменные остаются после вызова функции, сохраняя свои значения между её вызовами.
  static unsigned long   beginMillis;                                   // для таймера таймаута
  static unsigned char   Buf[RECV_BUFLEN];                              // буфер с данными в виде массива с размером 64
  static unsigned char * _bufPtr;                                       // Указатель на массив куда будут записаны данные

  _bufPtr = (unsigned char*)&Buf;                                       // _bufPtr = значение по адресу в памяти первого блока данных массива буфера данных (вроде так)

  switch (step) {                                                       // идем по шагам
    case 0:                                                             // шаг 0 - ищем преамбулу 5AA5
      while (NINEBOT_PORT.available() >= 2) {                            // пока в порту есть данные для чтения больше или = двух байт
        if (NINEBOT_PORT.read() == 0x5A && NINEBOT_PORT.peek() == 0xA5) { // если в порту есть 0x5A читаем его и он стирается из буфера, далее читаем первый байт с краю буфера и если он = A5
          NINEBOT_PORT.read();                                           // удаляем из буфера и его (т.е. дальше остаются уже заголовок и данные без преамбулы и контрольная сумма)
          step = 1;                                                     // переходим к следующему шагу 1
          break;                                                        // выходим из цикла while
        }
      }
      break;                                                            // выходим из switch case, нужно попробовать убрать
    case 1:                                                             // шаг 1 - преамбула получена, получаем тело пакета
      static unsigned char   readCounter;                               // чтение счетчик по байтно
      static unsigned int    _cs;                                       // контрольная сумма
      static unsigned char * bufPtr;                                    // Указатель на массив
      static unsigned char * asPtr; //                                  // Указатель на массив заголовка
      unsigned char bt;                                                 //
      if (entry) {    //init variables                                  // если переменная "вход" = 1 инициализируем переменные для получения нового пакета
        memset((void*)&AnswerHeader, 0, sizeof(AnswerHeader));          // заполняем область памяти по адресу массива AnswerHeader нулями в количестве sizeof(AnswerHeader)
        bufPtr = _bufPtr;                                               // Указатель на массив bufPtr=_bufPtr = значение по адресу в памяти первого блока данных массива буфера данных (вроде так)
        readCounter = 0;                                                // счетчик чтения = 0
        beginMillis = millis();                                         // засекаем время
        asPtr   = (unsigned char *)&AnswerHeader;   //pointer onto header structure  //Указатель на массив AnswerHeader
        _cs = 0xFFFF;                                                   // контрольная сумма = FFFF (макс)
      }
      if (readCounter >= RECV_BUFLEN) {                                 // если счетчик чтения превысил размер буфера в 64 байта
        step = 2;                                                       // переходим к следующему шагу 2
        break;                                                          // выходим из switch case
      }
      if (millis() - beginMillis >= RECV_TIMEOUT) { //timeout           // если превысили таймаут  RECV_TIMEOUT из defines.h
        step = 2;                                                       // переходим к следующему шагу 2
        break;                                                          // выходим из switch case
      }

      while (NINEBOT_PORT.available()) {                                 // пока в порту есть данные для чтения
        bt = NINEBOT_PORT.read();                                        // читаем очередной байт и присваеваем его переменной bt
        readCounter++;                                                   // прибавляем счетчик чтения на 1
        if (readCounter <= sizeof(AnswerHeader)) {  //читаем заголовок в asPtr // отделяем заголовок от структуры заголовка (пока счетчик чтения меньше размера массива заголовка)
          *asPtr++ = bt;                                                // указатель на очередной элемент заголовка равен байту из порта (прибавляется на 1) сначала присваеваем, потом прибавляем указатель на элемент массива
          _cs -= bt;                                                    // контрольная сумма уменьшается на текущий байт из буфера
        }
        if (readCounter > sizeof(AnswerHeader)) {   //now begin read data  // как прочитали заголовок начинаем читать данные(счетчик чтения превысил размер AnswerHeader)
          *bufPtr++ = bt;                  //читаем данные в bufPtr   // очередной элемент bufPtr равняетя текущему байту (сначала присваеваем, поотом увеличиваем порядковый номер
          if (readCounter < (AnswerHeader[0] + 3)) {                   // если счетчик чтения меньше считанного размера (по сруктуре AnswerHeader)len+3
            _cs -= bt;                                                  // контрольная сумма уменьшается на текущий байт из буфера
          }
        }
        beginMillis = millis();                     //reset timeout     // сбрасываем таймаут
      }

      if (AnswerHeader[0] == (readCounter - 4)) {  //if len in header == current received len  // если (по сруктуре AnswerHeader)len равен счетчик чтения - 4
        unsigned int   cs;                                              // переменная контрольной суммы
        unsigned int * ipcs;                                            // указатель на область памяти в массиве буфера где хранится контрольная суммв
        ipcs = (unsigned int*)(bufPtr - 2);                             // два блока с конца буфера - там хранится контрольная сумма
        cs = *ipcs;                                                     // переменная контррлольной суммы равна тем двум блокам в пакете
        if (cs != _cs) { //check cs                                     // если контрольная сумма не совпадает с посчитанной выше
          step = 2;                                                     // переходим к следующему шагу 2
          break;                                                        // выходим из switch case
        }
        //here cs is ok, header in AnswerHeader, data in _bufPtr или Buf // на данном этапе контрольная сумма ок, заголовок в AnswerHeader, данные в _bufPtr
        //processPacket(_bufPtr, readCounter);                            // вызываем функция распаковки с входными данными - массив данных _bufPtr и счетчик чтения
        for (int i = 0; i < sizeof(AnswerHeader); i++) {
          Serial.print(AnswerHeader[i], HEX);
          Serial.print(' ');
        }
        for (int i = 0; i < sizeof(Buf); i++) {
          Serial.print(Buf[i], HEX);
          Serial.print(' ');
        }
        Serial.println();
        step = 2;                                                       // переходим к следующему шагу 2
        break;                                                          // выходим из switch case
      }
      break; //case 1:                                                  // выходим из switch case
    case 2:  //end of receiving data                                    // следующий шаг - 2
      step = 0;                                                         // обнуляем счетчик шагов
      break;                                                            // выходим из switch case
  }

  if (_step != step) {
    _step = step;
    entry = 1;
  } else {
    entry = 0;
  }










  /* if (NINEBOT_PORT.available()) {                                  // если в порту есть данные
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
         if (curs < 2 || (curs >= 2 && curs < (9 + data[2]))) {    // если текущий элемент массива меньше 2 или больше 2 но меньше 9 + третий элемент массива (размер данных из пакета)
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
           //Serial.println("CS OK");
           Unpack(); // запускаем распаковку
         }


         state = 0;                                                 // обратно в состояние 0 - поиск заголовка
         break;                                                     // выходим из case и возвращемся в начало
     }
    }*/
}

void PrintPack() {
  for (int i = 0; i < curs; i++) {
    Serial.print(data[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}
