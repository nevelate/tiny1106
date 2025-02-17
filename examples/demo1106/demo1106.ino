//#define OLED_RU // undefine if you are using cyrilic characters
//#define OLED_OFFSET 0 // undefine if you are using 132x64 display

#include <tiny1106.h>

Oled oled;

void setup() {
  oled.init();
}

void loop() {
  printFastTest();

  printTest();

  drawLineTest();
  delay(2000);

  drawLineVTest();
  delay(2000);

  drawLineHTest();
  delay(2000);

  drawPointTest();
  delay(2000);
}

void printFastTest(){
  for(int i = 1; i <= 4; i++){
    oled.clear();
    oled.setTextScale(i);
    oled.printFast("Hi!");
    delay(1000);
  }
}

void printTest(){
  for(int i = 1; i <= 4; i++){
    oled.clear();
    oled.setCursor(20, 20);
    oled.setTextScale(i);
    oled.printFast("Hey");
    delay(1000);
  }
}

void drawLineTest(){
  oled.clear();
  for(int i = 0; i <= 10; i++){
    oled.drawLine(0, 0, 127, i * 6);
  }
  for(int i = 20; i >= 0; i--){
    oled.drawLine(0, 0, i * 6, 63);
  }
}

void drawLineVTest(){
  oled.clear();
  for(int i = 0; i < 32; i++){
    oled.drawLineV(i * 4, i, 32 + i);
  }
}

void drawLineHTest(){
  oled.clear();
  for(int i = 0; i < 16; i++){
    oled.drawLineH(i * 4, i * 4, 64 + i * 4);
  }
}

void drawPointTest(){
  oled.clear();
  for(int i = 0; i < 200; i++){
    oled.drawPoint(random(128), random(64));
  }
}