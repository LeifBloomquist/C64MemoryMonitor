// ----------------------------------------------------------
// C64 Visual Memory Monitor 

#pragma GCC optimize("-O3")

#include <Adafruit_NeoPixel.h>
#include <digitalWriteFast.h>

#define LEDS     64
#define PIN_NEO  A1 
#define PIN_PHI2 2   // INT0
#define PIN_RW   A0

// These are just here for documentation, since we read them directly using PINB and not with digitalReadFast()
// We can't use PORTD because it seems to always read 0 when INT0 is enabled, and we don't want to mess with USB Serial Rx/Tx anyway.

#define PIN_A15  13    // PB5 
#define PIN_A14  12    // PB4
#define PIN_A13  11    // PB3
#define PIN_A12  10    // PB2
#define PIN_A11  9     // PB1
#define PIN_A10  8     // PB0

volatile byte rw = 0;
//volatile unsigned long phi2_cnt = 0;
volatile int address1 = 0;
volatile int address2 = 0;

volatile byte reads[LEDS] = { 0 };
volatile byte writes[LEDS] = { 0 };

int counter = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN_NEO, NEO_GRB + NEO_KHZ800);

void setup(void)
{
  //Serial.begin(115200);

  pinModeFast(PIN_RW, INPUT);
  pinModeFast(PIN_PHI2, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_PHI2), phi2_isr, RISING);

  DDRB = B00000000;   //  All inputs for address lines

  // Enable the pullups for our six inputs
  PORTB = PORTB | B00111111;

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}


int red = 0;
int green = 0;
int i = 0;

void loop()
{
  for (i = 0; i < LEDS; i++)
  { 
    strip.setPixelColor(i, writes[i], reads[i], 0);

    writes[i] = writes[i] >> 1;   // Fade the color out by shifting right
    reads[i]  = reads[i]  >> 1;
  }

//  counter = !counter;
//  strip.setPixelColor(59, 0, 0, counter*255);
  strip.show();
}

/*
void loop_debug()
{
  Serial.println(phi2_cnt);
  phi2_cnt = 0;

  for (int i = 0; i < LEDS; i++)
  {
    Serial.print(reads[i]);
    reads[i] = 0;
    
    Serial.print("  ");
  }
  Serial.println();

  for (int i = 0; i < LEDS; i++)
  {
    Serial.print(writes[i]);
    writes[i] = 0;

    Serial.print("  ");
  }
  Serial.println();
  Serial.println();

  delay(1000);
}
*/

// ISR

void phi2_isr()
{ 
  // This crazy logic filters out noise? on the expansion port
  address1 = PINB & B00111111;  // Invert (active low) and mask out the crystal inputs on upper two bits
  address2 = PINB & B00111111;  // Invert (active low) and mask out the crystal inputs on upper two bits
  if (address1 != address2) return;  

  rw = digitalReadFast(PIN_RW);

  if (rw)
  {
    reads[address1]=255;   // High is a read
  }
  else
  {
    writes[address1]=255;  // Low is a write
  }

  //phi2_cnt++;
}

// #EOF