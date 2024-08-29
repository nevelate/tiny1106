//#define OLED_RU // undefine if you are using cyrilic characters
//#define OLED_OFFSET 0 // undefine if you are using 132x64 display

#include <tiny1106.h>

Oled oled;

void setup() {
  oled.init();
}

void loop() {
  delay(2000);
  oled.drawLine(10, 10, 40, 40);
  delay(2000);
  oled.drawLineH(32, 10, 110);
  delay(2000);
  oled.drawLineV(64, 20, 50);
  delay(2000);
  oled.clear();
  oled.print(20, 20, "Hello, World!");
}