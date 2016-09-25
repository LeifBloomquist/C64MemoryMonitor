// ----------------------------------------------------------
// C64 Visual Memory Monitor 

#include <digitalWriteFast.h>

#define PIN_PHI2 2   // INT0
#define PIN_RW   3    
#define PIN_A15  4
#define PIN_A14  5
#define PIN_A13  6
#define PIN_A12  7
#define PIN_A11  8
#define PIN_A10  9

volatile unsigned long phi2_cnt = 0;
volatile unsigned int address = 0;

volatile int rw = 0;
volatile unsigned long reads[64] = { 0 };
volatile unsigned long writes[64] = { 0 };

void setup(void)
{
  Serial.begin(115200);

  pinModeFast(PIN_RW , INPUT);
  pinModeFast(PIN_A15, INPUT);
  pinModeFast(PIN_A14, INPUT);
  pinModeFast(PIN_A13, INPUT);
  pinModeFast(PIN_A12, INPUT);
  pinModeFast(PIN_A11, INPUT);
  pinModeFast(PIN_A10, INPUT);

  pinMode(PIN_PHI2, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_PHI2), phi2_isr, RISING);
}

void loop()
{
  //Serial.println(phi2_cnt);
  phi2_cnt = 0;

  unsigned long total = 0;

  for (int i = 0; i < 64; i++)
  {
    Serial.print(reads[i]);
    reads[i] = 0;
    
    Serial.print("  ");
  }

  Serial.println();

  for (int i = 0; i < 64; i++)
  {
    Serial.print(writes[i]);
    writes[i] = 0;

    Serial.print("  ");
  }

  Serial.println();
  Serial.println();

  delay(100);
}

// ISR

void phi2_isr()
{
  rw = digitalReadFast(PIN_RW);
   
  address = (!digitalReadFast(PIN_A15) << 5)
          | (!digitalReadFast(PIN_A14) << 4)
          | (!digitalReadFast(PIN_A13) << 3)
          | (!digitalReadFast(PIN_A12) << 2)
          | (!digitalReadFast(PIN_A11) << 1)
          | (!digitalReadFast(PIN_A10) << 0);

  if (rw)
  {
    reads[address]++;   // High is a read
  }
  else
  {
    writes[address]++;  // Low is a write
  }

  phi2_cnt++;
}

// #EOF
