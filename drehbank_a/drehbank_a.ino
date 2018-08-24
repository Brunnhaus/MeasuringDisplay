#include <TM1638.h>

TM1638 anzeige[] = {
  TM1638(13, 5, 6),
  TM1638(13, 5, 12),
  TM1638(13, 5, 7)
};

const byte clocks[] = {
  A2, A3, A4, A5
};
const byte data[] = {
  8, 2, 3, 4
};

int bit_array[25];
unsigned long time_now;
unsigned long signal_delay;
unsigned long tmp_time;

float x_data[2];

void setup() {
  for (byte x : clocks) pinMode(x, INPUT);
  for (byte x : data) pinMode(x, INPUT);
}

float usb_decode(byte dat, byte clk) {
  int sign = 1;
  byte c = 0;
  float value = 0.0;
  bit_array[c] = 1;
  while (digitalRead(clk) == HIGH) {};
  for (c = 1; c <= 21; c++) {
    tmp_time = millis();
    while (digitalRead(clk) == LOW) {
      if (millis() - tmp_time > 100) {
        return 0;
        break;
      }
    }
    bit_array[c] = digitalRead(dat);
    while (digitalRead(clk) == HIGH) {}
  }
  for (c = 1; c <= 20; c++) {
    value = value + (pow(2, c - 1) * bit_array[c]);
  }
  if (bit_array[21] == 1) sign = -1;
  return ((value * sign * -2) / 100.00);
}

String out;
String convert(float in) {
  String out = " ";
  if (in > -100) out += " ";
  if ((in < 100) && (in >= 0)) out += " ";
  if ((in < 10) && (in > -10)) out += " ";
  return out + String(in);
}

void loop() {
  for (byte x = 0; x < 4; x++) {
    signal_delay = millis();
    while (digitalRead(clocks[x]) == LOW) {
      if (millis() - signal_delay > 100) {
        if (x <= 1) {
          x_data[x] = 0;
          anzeige[0].setDisplayToString(convert(x_data[!x]));
        }
        else anzeige[x-1].setDisplayToString("--------");
        break;
      }
    }
    time_now = micros();
    while (digitalRead(clocks[x]) == HIGH) {}
    if ((micros() - time_now) > 500) {
      if (x <= 1) {
        x_data[x] = usb_decode(data[x], clocks[x]);
        anzeige[0].setDisplayToString(convert(x_data[0] + x_data[1]));
      }
      else if (x == 2) {
        anzeige[1].setDisplayToString(convert((-2 * usb_decode(data[x], clocks[x])));
      }
      else {
        anzeige[x-1].setDisplayToString(convert(usb_decode(data[x], clocks[x])));
      }
    }
  }
}
