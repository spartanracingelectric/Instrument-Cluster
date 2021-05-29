#include <ArduinoQueue.h>
#include <avr/interrupt.h>
#include "conf.h"
#include "indicators.h"
#include "1602lcd.h"
#include "buttons.h"
#include "can.h"

#define QUEUE_MAX_SIZE 100

signed int RPM;
float SOC, TEMP;

can_message input;
ArduinoQueue<can_message> messages(QUEUE_MAX_SIZE);

void setup() {
  LCD__init();  // Serial.begin(9600);
  CAN_initialize();
  buttons__init();

  LCD__wake();
  LCD__default();
  rpm__set(0b1111111111111111);
  left_rgb__set(BLACK);
  right_rgb__set(BLACK);
  left_indicator__set(1);   // NOTE THAT 1 = OFF. 0 = ON
  right_indicator__set(1);  // NOTE THAT 1 = OFF. 0 = ON
}

void loop() {
  input = CAN__receive_packet();

  // This should never happen
  // If the messages queue is full, empty the queue
  if (messages.isFull()) {
    for (int i = 0; i < QUEUE_MAX_SIZE; i++) {
      messages.dequeue();
    }
  }
  
  messages.enqueue(input);
  
  switch (messages.getHead().id) {
  case SOC_ADDR:
    SOC = CAN__convert_SOC(messages.dequeue());
    break;
  case BAT_TEMP_ADDR:
    TEMP = CAN__convert_TEMP(messages.dequeue());
    break;
  case RPM_ADDR:  
    RPM = CAN__convert_RPM(messages.dequeue());
    break;
  default:
    messages.dequeue();
    break;
  }
  
  indicator__update(RPM, SOC, TEMP);
  LCD__update(SOC, TEMP);
  buttons__poll();
  buttons__update_LCD();
  
  /*
  RPM = (signed int)CAN__receive_RPM();
  SOC = CAN__receive_SOC();
  TEMP = CAN__receive_TEMP();

  Serial.println("******************************************************************************");
  Serial.print("RPM:\t");
  Serial.println(RPM);
  Serial.print("SOC:\t");
  Serial.println(SOC);
  Serial.print("TEMP:\t");
  Serial.println(TEMP);

  indicator__update(RPM, SOC, TEMP);
  
  buttons__poll();
  buttons__update_LCD();
  */

  /*
  // Use this stuff to test the leds
  // in this case, 1 = ON; 0 = OFF
  delay(300);
  rpm__set(0b0000000011111111);
  delay(300);
  rpm__set(0b1111111100000000);
  delay(300);

  // 1 = OFF; 0 = ON 
  uint16_t status = 0b1111111111111111;

  for (int i = 0; i < 16; i++) {
    status &= ~(1<<i);
    rpm__set(status);
    delay(100);

  }
  
  for (int i = 16; i >= 0; i--) {
    status |= (1<<i);
    rpm__set(status);
    delay(100);
  } 
  
  delay(300);
  left_indicator__set(0);
  delay(300);
  right_indicator__set(0);
  delay(300);
  left_rgb__set(RED);
  delay(300);
  left_rgb__set(WHITE);
  delay(300);
  left_rgb__set(BLUE);
  delay(300);
  left_rgb__set(GREEN);
  delay(300);
  right_rgb__set(RED);
  delay(300);
  right_rgb__set(WHITE);
  delay(300);
  right_rgb__set(BLUE);
  delay(300);
  right_rgb__set(GREEN);
  delay(300);
  right_rgb__set(BLACK);
  delay(300);
  left_rgb__set(BLACK);
  delay(300);
  left_indicator__set(1);
  delay(300);
  right_indicator__set(1);
  delay(300);
  */
}
