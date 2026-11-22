const int PIN_SENSOR = 34;
const int PIN_LASER  = 27;
const int PIN_LED    = 25;

// ====== PENGATURAN ======
const int THRESHOLD = 100;             
const unsigned long HOLD_MS  = 2500;    
const unsigned long BLINK_MS = 250;     
const bool DEBUG_SERIAL = true;         
// =========================

int baseline = 0;
unsigned long lastBreak = 0;
unsigned long lastBlink = 0;
unsigned long lastDebug = 0;
bool ledState = false;

int readAvg(int n) {
  long sum = 0;
  for (int i = 0; i < n; i++) sum += analogRead(PIN_SENSOR);
  return sum / n;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LASER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  analogSetPinAttenuation(PIN_SENSOR, ADC_11db);

  digitalWrite(PIN_LASER, HIGH);   // laser nyala terus
  delay(1000);                     

  baseline = readAvg(64);
  Serial.printf("Baseline sensor: %d\n", baseline);
  if (baseline < 100 || baseline > 3995) {
    Serial.println("PERINGATAN: nilai baseline ekstrem. Cek arah laser ke sensor, dan wiring AO.");
  }
}

void loop() {
  int value = readAvg(8);
  bool broken = value < (baseline - THRESHOLD);

  if (broken) lastBreak = millis();

  bool alarm = (lastBreak != 0) && (millis() - lastBreak < HOLD_MS);

  if (alarm) {
    if (millis() - lastBlink >= BLINK_MS) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(PIN_LED, ledState);
    }
  } else {
    ledState = false;
    digitalWrite(PIN_LED, LOW);
  }

  if (DEBUG_SERIAL && millis() - lastDebug > 300) {
    lastDebug = millis();
    Serial.printf("sensor=%d baseline=%d %s\n", value, baseline, broken ? "TERHALANG" : "aman");
  }

  delay(5);
}
