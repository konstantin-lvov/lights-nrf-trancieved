/*   Данный скетч делает следующее: передатчик (TX) отправляет массив
     данных, который генерируется согласно показаниям с кнопки и с
     двух потенциомтеров. Приёмник (RX) получает массив, и записывает
     данные на реле, сервомашинку и генерирует ШИМ сигнал на транзистор.
    by AlexGyver 2016
*/
#define interruptPin 2

#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля
#include <avr/sleep.h>

byte btn1, btn2, btn3;

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

byte wichBtn;

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик

  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  delay(100);
  attachInterrupt(0, wake_up, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

}

void loop() {
  if(btn1 == 0 || btn2 == 0 || btn3 == 0){
    if(btn1 == 0){
      wichBtn = 1;
    } else if (btn2 == 0){
      wichBtn = 2;
    } else if (btn3 == 0){
      wichBtn = 3;
    }
    Serial.print("sent :"); Serial.println(wichBtn);
    radio.write(&wichBtn, sizeof(wichBtn));
  }
  
  going_to_sleep();
}

void going_to_sleep() {
  sleep_enable();
  delay(30);
  Serial.println("going to sleep");
  sleep_cpu();
  btn1 = digitalRead(4);
  btn2 = digitalRead(5);
  btn3 = digitalRead(6);
  delay(100);
  Serial.println("woke up");
}

void wake_up() {
  sleep_disable();
}
