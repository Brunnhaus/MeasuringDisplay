#include <TM1638.h>

TM1638 anzeige[] = {
  TM1638(13, 5, 6),
  TM1638(13, 5, 12),
  TM1638(13, 5, 7)
};

const byte clocks[] = {
  A3, A4, A5
};
const byte data[] = {
  2, 3, 4
};

int bit_array[25];        // For storing the data bit. bit_array[0] = data bit 1 (LSB), bit_array[23] = data bit 24 (MSB).
unsigned long time_now;   // For storing the time when the clock signal is changed from HIGH to LOW (falling edge trigger of data output).
unsigned long signal_delay;
unsigned long tmp_time;

void setup() {
  for (byte x : clocks) pinMode(x, INPUT);
  for (byte x : data) pinMode(x, INPUT);
}

String usb_decode(byte dat, byte clk) {
  int sign = 1;
  int c = 0;
  float value = 0.0;
  bit_array[c] = 1;//digitalRead(dat);       // Store the 1st bit (start bit) which is always 1.
  while (digitalRead(clk) == HIGH) {};
  for (c = 1; c <= 24; c++) {
    tmp_time = millis();
    while (digitalRead(clk) == LOW) {
      if (millis() - tmp_time > 100) {
        return "--------";
        break;
      }
    }
    bit_array[c] = digitalRead(dat);
    while (digitalRead(clk) == HIGH) {} // Wait until clock returns to LOW
  }
  for (c = 1; c <= 20; c++) {                 // Turning the value in the bit array from binary to decimal.
    value = value + (pow(2, c - 1) * bit_array[c]);
  }
  if (bit_array[21] == 1) sign = -1;          // Bit 21 is the sign bit. 0 -> +, 1 => -
  return convert((value * sign) / 100.00);
}

String out;
String convert(float in) {
  out = " ";
  if (in > -100) out += " ";
  if ((in < 100) && (in >= 0)) out += " ";
  if ((in < 10) && (in > -10)) out += " ";
  return out + String(in);
}

void loop() {
  for (int x = 0; x < 3; x++) {
    signal_delay = millis();
    while (digitalRead(clocks[x]) == LOW) {
      if (millis() - signal_delay > 100) {
        anzeige[x].setDisplayToString("--------");
        break;
      }
    }
    time_now = micros();
    while (digitalRead(clocks[x]) == HIGH) {}
    if ((micros() - time_now) > 500) {
      anzeige[x].setDisplayToString(usb_decode(data[x], clocks[x]));
    }
  }
}
