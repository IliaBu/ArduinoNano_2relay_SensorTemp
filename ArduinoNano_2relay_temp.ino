#include <SoftwareSerial.h>
#include <OneWire.h>
#include "GyverTimer.h"
GTimer myTimer1;                // создать таймер для перезагрузки ардуино
GTimer myTimer2;                // создать таймер для реле

OneWire ds(6);
SoftwareSerial SIM800(2, 3); // установка контактов 2 и 3 для программного порта
const byte in1 = 10;          // Указываем, что вывод реле In1, подключен к реле цифровому выводу 10
int in2 = 8;              // Указываем, что вывод реле In2, подключен к реле цифровому выводу 8

 String sendATCommand(String cmd, bool waiting) {
  String _resp = "";                            // Переменная для хранения результата
  SIM800.println(cmd);                          // Отправляем команду модулю
  if (waiting) {                                // Если необходимо дождаться ответа...
    _resp = waitResponse();                     // ... ждем, когда будет передан ответ
    // Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
    if (_resp.startsWith(cmd)) {                // Убираем из ответа дублирующуюся команду
      _resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
    }
  }
  return _resp;                                 // Возвращаем результат. Пусто, если проблема
}

String waitResponse() {                         // Функция ожидания ответа и возврата полученного результата
  String _resp = "";                            // Переменная для хранения результата
  long _timeout = millis() + 10000;             // Переменная для отслеживания таймаута (10 секунд)
  while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 10 секунд, если пришел ответ или наступил таймаут, то...
  if (SIM800.available()) {                     // Если есть, что считывать...
    _resp = SIM800.readString();                // ... считываем и запоминаем
  }
  else {                                        // Если пришел таймаут, то...
    //Serial.println("Timeout...");               // ... оповещаем об этом и...
  }
  return _resp;                                 // ... возвращаем результат. Пусто, если проблема
}

void setup() {
 pinMode(in1, OUTPUT);    // Установим вывод 10 как выход
 pinMode(in2, OUTPUT);    // Установим вывод 8 как выход
 digitalWrite(in1, HIGH);
 delay(800);
 digitalWrite(in2, HIGH);
 delay(2000);
 digitalWrite(in2, LOW);
 //Serial.begin(19200);                   
 SIM800.begin(9600); 
 while(!SIM800.available()){             // Зацикливаем и ждем инициализацию SIM800L
    sendATCommand("AT", true);            // Отправили AT для настройки скорости обмена данными
    delay(1000);                             // Пауза
    }
    sendATCommand("AT+CMGF=1\r", true);   
    delay(500); 
    sendATCommand("AT+IFC=1, 1\r", true);                            
    delay(500); 
    sendATCommand("AT+CPBS=\"SM\"\r", true); 
    delay(500);
    sendATCommand("AT+CNMI=1,2,2,1,0\r", true);                                          
    delay(200); 
    myTimer1.setInterval(86400000);   // 86400000 настроить интервал 1 день для перезагрузки ардуино
    myTimer1.setMode(AUTO);  
    myTimer2.setInterval(1800000);   // настроить интервал 30 мин. для отключения реле 1
    myTimer2.setMode(AUTO);     
}

String currStr = "";   // Переменная для хранения ответа модуля
String _response = ""; // Переменная для хранения ответа модуля
// если эта строка сообщение, то переменная примет значение True
boolean isStringMessage = false;

void loop() {
  resetFunc();
  relayTimer();
 if (!SIM800.available()) // Если модем, что-то отправил...
 return;
 char currSymb = SIM800.read();
 if ('\r' == currSymb) {
 if (isStringMessage) {
 // если текущая строка – сообщение, то…
 if (!currStr.compareTo("1")) { // если получено смс "1", то включаем реле 1 на 30 мин.
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(1500);
 SIM800.print("Rele ON 30 min");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 myTimer2.reset();
 digitalWrite(in1, LOW);
 } else if (!currStr.compareTo("0")) { // если получено смс "0", то выключаем реле 1
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(1500);
 SIM800.print("Rele OFF");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 digitalWrite(in1, HIGH);
 } else if (!currStr.compareTo("8")) { // если получено смс "8", то выключаем реле 1
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(1500);
 SIM800.print("Rele OFF");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 digitalWrite(in1, HIGH);
 } else if (!currStr.compareTo("2")) { // если получено смс "2", то включаем реле 1 на 30 мин.
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(1500);
 SIM800.print("Rele ON 30 min");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 myTimer2.reset();
 digitalWrite(in1, LOW);
 } else if (!currStr.compareTo("3")) { // если получено смс "3", то включаем реле 1 на 1 сек.
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(1500);
 SIM800.print("Rele ON 2 sec");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 digitalWrite(in1, LOW);
 delay(2000);
 digitalWrite(in1, HIGH);
 } else if (!currStr.compareTo("4")) {  // если получено смс "4" то включаем реле 1 на 1 сек.
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(1500);
 SIM800.print("Rele ON 2 sec");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 digitalWrite(in1, LOW);
 delay(2000);
 digitalWrite(in1, HIGH);
 } else if (!currStr.compareTo("5")) {  // если получено смс "5" то запрос температуры
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
  byte data[2];
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
 delay(1500);
  ds.reset();
  ds.write(0xCC);
  ds.write(0xBE);
  float celsius;
  celsius = (ds.read() | (ds.read()<<8))/16.0;
 SIM800.print("t= "); SIM800.print(celsius); SIM800.print(" *C");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 } else if (!currStr.compareTo("6")) {  // если получено смс "6" то запрос температуры
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
  byte data[2];
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
 delay(1500);
  ds.reset();
  ds.write(0xCC);
  ds.write(0xBE);
  float celsius;
  celsius = (ds.read() | (ds.read()<<8))/16.0;
 SIM800.print("t= "); SIM800.print(celsius); SIM800.print(" *C");
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
 } else if (!currStr.compareTo("7")) {  // если получено смс "7" то сброс
 asm volatile("jmp 0x00");
 } 
 else if (!currStr.compareTo("9")) {  // если получено смс "9" то запрос баланса
 balance();
 }
 isStringMessage = false;
 } 
 else 
 {
 if (currStr.startsWith("+CMT")) {
 // если текущая строка начинается с "+CMT", то следующая сообщение
 isStringMessage = true;
 }
 if (currStr.startsWith("+CUSD")) {       // Пришло уведомление о USSD-ответе
      String msgBalance = currStr.substring(currStr.indexOf("\"") + 1);  // Получаем непосредственно содержимое ответа
      msgBalance = msgBalance.substring(0, msgBalance.indexOf("\""));
      // Ответ в UCS2-формате - декодируем и извлекаем число
      msgBalance = UCS2ToString(msgBalance);          // Декодируем ответ
      float balance = getFloatFromString(msgBalance);   // Парсим ответ на содержание числа
      smsSendAlarm1("Balance: " + String(balance));
  }
 }
 currStr = "";
 } else if ('\n' != currSymb) {
 currStr += String(currSymb);
 }
}

void resetFunc() {
  if (myTimer1.isReady()) {
  asm volatile("jmp 0x00");
  }
}

void relayTimer() {
  if (myTimer2.isReady()) {
  digitalWrite(in1, HIGH);
  }
}

void smsSendAlarm1(String text) 
{
 SIM800.print("AT+CMGS="); // отправляем смс
 SIM800.print((char)34);
 SIM800.print("+71111111111"); // номер получателя
 SIM800.print((char)34); // символ кавычки "
 SIM800.print((char)13); // символ <cr> (ASCII character 13)
 delay(300);
 SIM800.print(text);
 text = "";
 delay(500);
 SIM800.print((char)26); // символ <ctrl-z> (ASCII character 26)
}

void balance(){
SIM800.print("AT+CUSD=1,");
SIM800.print('"');
SIM800.print("*102#");  //USD номер запроса баланса оператора сотовой связи
SIM800.println('"');
}

String UCS2ToString(String s) {                       // Функция декодирования UCS2 строки
  String result = "";
  unsigned char c[5] = "";                            // Массив для хранения результата
  for (int i = 0; i < s.length() - 3; i += 4) {       // Перебираем по 4 символа кодировки
    unsigned long code = (((unsigned int)HexSymbolToChar(s[i])) << 12) +    // Получаем UNICODE-код символа из HEX представления
                         (((unsigned int)HexSymbolToChar(s[i + 1])) << 8) +
                         (((unsigned int)HexSymbolToChar(s[i + 2])) << 4) +
                         ((unsigned int)HexSymbolToChar(s[i + 3]));
    if (code <= 0x7F) {                               // Теперь в соответствии с количеством байт формируем символ
      c[0] = (char)code;
      c[1] = 0;                                       // Не забываем про завершающий ноль
    } else if (code <= 0x7FF) {
      c[0] = (char)(0xC0 | (code >> 6));
      c[1] = (char)(0x80 | (code & 0x3F));
      c[2] = 0;
    } else if (code <= 0xFFFF) {
      c[0] = (char)(0xE0 | (code >> 12));
      c[1] = (char)(0x80 | ((code >> 6) & 0x3F));
      c[2] = (char)(0x80 | (code & 0x3F));
      c[3] = 0;
    } else if (code <= 0x1FFFFF) {
      c[0] = (char)(0xE0 | (code >> 18));
      c[1] = (char)(0xE0 | ((code >> 12) & 0x3F));
      c[2] = (char)(0x80 | ((code >> 6) & 0x3F));
      c[3] = (char)(0x80 | (code & 0x3F));
      c[4] = 0;
    }
    result += String((char*)c);                       // Добавляем полученный символ к результату
  }
  return (result);
}

unsigned char HexSymbolToChar(char c) {
  if      ((c >= 0x30) && (c <= 0x39)) return (c - 0x30);
  else if ((c >= 'A') && (c <= 'F'))   return (c - 'A' + 10);
  else                                 return (0);
}

float getFloatFromString(String str) {            // Функция извлечения цифр из сообщения - для парсинга баланса из USSD-запроса
  bool   flag     = false;
  String result   = "";
  str.replace(",", ".");                          // Если в качестве разделителя десятичных используется запятая - меняем её на точку.
  for (int i = 0; i < str.length(); i++) {
    if (isDigit(str[i]) || (str[i] == (char)46 && flag)) { // Если начинается группа цифр (при этом, на точку без цифр не обращаем внимания),
      result += str[i];                           // начинаем собирать их вместе
      if (!flag) flag = true;                     // Выставляем флаг, который указывает на то, что сборка числа началась.
    }
    else {                                        // Если цифры закончились и флаг говорит о том, что сборка уже была,
      if (flag) break;                            // считаем, что все.
    }
  }
  return result.toFloat();                        // Возвращаем полученное число.
}
