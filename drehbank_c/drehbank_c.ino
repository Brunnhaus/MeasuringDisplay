#include <TM1638.h>

#define anzeigen 4

TM1638 anzeige[] = {
  TM1638(13, 5, 6),
  TM1638(13, 5, 12),
  TM1638(13, 5, 7)
};

const byte clocks[] = {
  A3, A4, A5, A2
};
const byte data[] = {
  2, 3, 4, 8
};

int temp_x = 0;

int bit_array[25];        // For storing the data bit. bit_array[0] = data bit 1 (LSB), bit_array[23] = data bit 24 (MSB).
unsigned long time_now;   // For storing the time when the clock signal is changed from HIGH to LOW (falling edge trigger of data output).

unsigned long signal_delay;

void setup() {
  Serial.begin(9600);
  for (byte x : clocks) pinMode(x, INPUT);
  for (byte x : data) pinMode(x, INPUT);
}

String decode(byte dat, byte clk) {
  int sign = 1;
  int c = 0;
  float value = 0.0;
  float result = 0;
  String res;

  bit_array[c] = digitalRead(dat);       // Store the 1st bit (start bit) which is always 1.
  while (digitalRead(clk) == HIGH) {};

  for (c = 1; c <= 24; c++) {
    while (digitalRead(clk) == LOW) { } // Wait until clock returns to HIGH
    bit_array[c] = digitalRead(dat);
    while (digitalRead(clk) == HIGH) {} // Wait until clock returns to LOW
  }

  for (c = 1; c <= 20; c++) {                 // Turning the value in the bit array from binary to decimal.
      value = value + (pow(2, c-1) * bit_array[c]);
  }

  if (bit_array[21] == 1) sign = -1;          // Bit 21 is the sign bit. 0 -> +, 1 => -
  if (bit_array[24] == 1) {                     // Bit 24 tells the measureing unit (1 -> in, 0 -> mm)
    result = (value*sign) / 2000.00;
    res = "in " + String(result);
  }
  else {
    result = (value*sign) / 100.00;

    if (dat == 2) {
      temp_x = result;
    }
    if (dat == 8) {
      result += temp_x;
    }

    res = " ";
    if (result > -100) res += " ";
    if ((result < 100) && (result >= 0)) res += " ";
    if ((result < 10) && (result > -10)) res += " ";
    res += String(result);
    /*ALTERNATIV:
    String res;
    for (int x = String(result).length(); x < 8; x++) res +=" ";
    res += String(result);
    */
  }
  return res;
}

void loop() {
  for (int x = 0; x < anzeigen; x++) {
    signal_delay = millis();
    while (digitalRead(clocks[x]) == LOW) {
     if (signal_delay - millis() > 100) {
      anzeige[x].setDisplayToString("--------");
      break;
     }
    }
    time_now = micros();
    while (digitalRead(clocks[x]) == HIGH) {} // Wait for the end of the HIGH pulse
    if ((micros() - time_now) > 500) {        // If the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
    byte i = x;
    if (i == 3) i=0;
    anzeige[i].setDisplayToString(decode(data[x], clocks[x]));
  }
  /*
   while (digitalRead(clocks[0]) == LOW)
    {}  // If clock is LOW wait until it turns to HIGH
  time_now = micros();
  while (digitalRead(clocks[0]) == HIGH)
    {} // Wait for the end of the HIGH pulse
  if ((micros() - time_now) > 500)
    {        // If the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
    anzeige[0].setDisplayToString(decode(data[0], clocks[0]));
    Serial.println("daten");
    }*/

}
