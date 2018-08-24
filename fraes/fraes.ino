#include <TM1638.h>

TM1638 X_LED(13, 5, 6);

const byte X_CLOCK_PIN = A3;
const byte X_DATA_PIN = 2jhl;

int bit_array[25];        // For storing the data bit. bit_array[0] = data bit 1 (LSB), bit_array[23] = data bit 24 (MSB).
unsigned long time_now;   // For storing the time when the clock signal is changed from HIGH to LOW (falling edge trigger of data output).

void setup(void)
{
  pinMode(X_CLOCK_PIN, INPUT);
  pinMode(X_DATA_PIN, INPUT);
}

void decode(byte data, byte clk) {
  int sign = 1;
  int c = 0;
  float value = 0.0;
  float result = 0;

  bit_array[c] = digitalRead(data);       // Store the 1st bit (start bit) which is always 1.
  while (digitalRead(clk) == HIGH) {};

  for (c = 1; c <= 24; c++) {
    while (digitalRead(clk) == LOW) { } // Wait until clock returns to HIGH
    bit_array[c] = digitalRead(data);
    while (digitalRead(clk) == HIGH) {} // Wait until clock returns to LOW
  }

  for (c = 1; c <= 20; c++) {                 // Turning the value in the bit array from binary to decimal.
      value = value + (pow(2, c-1) * bit_array[c]);
  }

  if (bit_array[21] == 1) sign = -1;          // Bit 21 is the sign bit. 0 -> +, 1 => -

  if (bit_array[24] == 1)                     // Bit 24 tells the measureing unit (1 -> in, 0 -> mm)
     {
     result = (value*sign) / 2000.00;
     String res = "in " + String(result);
     X_LED.setDisplayToString(res);
     }
    else
     {
     result = (value*sign) / 100.00;
     String res = " ";
     if (result > -100) res += " ";
     if ((result < 100) && (result >= 0)) res += " ";
     if ((result < 10) && (result > -10)) res += " ";
     res += String(result);
     //ALTERNATIV:
     /*String res;
     for (int x = String(result).length(); x < 8; x++) res +=" ";
     res += String(result);*/
     //
     X_LED.setDisplayToString(res); // IN MILLIMETERN
     }
}

void loop()
{
 while (digitalRead(X_CLOCK_PIN) == LOW)
    {}  // If clock is LOW wait until it turns to HIGH
  time_now = micros();
  while (digitalRead(X_CLOCK_PIN) == HIGH)
    {} // Wait for the end of the HIGH pulse
  if ((micros() - time_now) > 500)
    {        // If the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
    decode(X_DATA_PIN, X_CLOCK_PIN);
    }
    /*

  while (digitalRead(Y_CLOCK_PIN) == LOW)
    {}  // If clock is LOW wait until it turns to HIGH
  time_now = micros();
  while (digitalRead(Y_CLOCK_PIN) == HIGH)
    {} // Wait for the end of the HIGH pulse
  if ((micros() - time_now) > 500)
    {        // If the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
    decode(Y_DATA_PIN, Y_CLOCK_PIN);
    }

 /*  while (digitalRead(Z_CLOCK_PIN) == LOW)
    {}  // If clock is LOW wait until it turns to HIGH
  time_now = micros();
  while (digitalRead(Z_CLOCK_PIN) == HIGH)
    {} // Wait for the end of the HIGH pulse
  if ((micros() - time_now) > 500)
    {        // If the HIGH pulse was longer than 500 micros we are at the start of a new bit sequence
    decode(Z_DATA_PIN, Z_CLOCK_PIN);
    } */
}
