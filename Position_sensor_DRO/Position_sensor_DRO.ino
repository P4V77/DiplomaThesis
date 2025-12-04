const int pinA = 2, pinB = 3;
const int phaseLookup[] = {0, -1, 1, -1, 1, 0, -1, -1, -1, 1, 0, 1, 1, 1, -1, 0};

volatile bool A = false, B = false;
volatile long counter = 0; 
volatile int phase = 0;

unsigned long lastPrintTime = 0; 

void setup()
{
  Serial.begin(230400);
  
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(pinA), trig, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinB), trig, CHANGE);
}

void trig()
{
  uint8_t newPhase = (digitalRead(pinA) << 1) | digitalRead(pinB);

  // Check for valid transitions
  uint8_t expectedPhase = (phase & 0x3); // Last two bits of the phase
  if (((expectedPhase == 0b00) && (newPhase == 0b01 || newPhase == 0b10)) ||
      ((expectedPhase == 0b01) && (newPhase == 0b11 || newPhase == 0b00)) ||
      ((expectedPhase == 0b11) && (newPhase == 0b10 || newPhase == 0b01)) ||
      ((expectedPhase == 0b10) && (newPhase == 0b00 || newPhase == 0b11)))
  {
    phase = ((phase << 2) | newPhase) & 0xF;
    counter += phaseLookup[phase];
  }
}

  
void loop()
{
  unsigned long currentTime = millis(); 
  if (currentTime - lastPrintTime >= 10) // Update faster
  {
    Serial.println(float(-counter * 0.005)); 
    lastPrintTime = currentTime; 
  }
}
