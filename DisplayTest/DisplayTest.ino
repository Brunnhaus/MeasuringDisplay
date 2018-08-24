#include <TM1638.h>

TM1638 x(8, 7, 6);
TM1638 y(8, 7, 5);
TM1638 z(8, 7, 4);

unsigned long a = 12345;
//byte values[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

byte values[] = { 118, 0, 0, 0, 0, 0, 0, 0};

void setup() {
   x.setDisplayToString("FIRST   ");
   y.setDisplayToString("SECOND  ");
   z.setDisplayToString("THIRD   ");
}

void loop() {}

/*
X 118
Y 102
Z 91
 */
