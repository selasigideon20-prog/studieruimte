#define BLYNK_TEMPLATE_ID "TMPL5Eu5Xzma5"
#define BLYNK_TEMPLATE_NAME "studieruimte"
#define BLYNK_AUTH_TOKEN "dqnC16epvgg8S1Uj4-rdwH4lEaUSZHQg"

#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BlynkSimpleEsp32.h>

#define IN_SENSOR    4
#define OUT_SENSOR   19
#define SOUND_PIN    2

#define RED_LED      21
#define GREEN_LED    20
#define BLUE_LED     22
#define BUZZER       8
#define BUTTON_PIN   10

LiquidCrystal lcd(12, 5, 6, 7, 15, 18);

// WiFi
const char* ssid = "";
const char* password = "";

WebServer server(80);

int aantal = 0;
const int MAX_PERSONEN = 5;

// sensor states
bool inArmed = true;
bool outArmed = true;

unsigned long lastInTrigger = 0;
unsigned long lastOutTrigger = 0;
const unsigned long sensorCooldown = 1500;

// buzzer cooldown
unsigned long lastBeep = 0;
const int beepCooldown = 1200;

// niet storen
bool nietStoren = false;
bool lastButtonState = HIGH;
unsigned long lastButtonTime = 0;

// lawaai
bool lawaaiMelding = false;
unsigned long lawaaiTot = 0;
int lawaaiLevel = 0;
unsigned long laatsteLawaaiTijd = 0;
const unsigned long lawaaiResetTijd = 15000;
unsigned long geluidBlokkerenTot = 0;

// automatische rustwaarde geluid
int rustGeluid = 0;
const int geluidDrempel = 80;   // probeer 80, verhoog naar 100 of 120 als nog te gevoelig

// tijdelijke melding
String lijn1 = "";
String lijn2 = "";
unsigned long meldingTot = 0;

// Blynk timing
unsigned long lastBlynkUpdate = 0;
const unsigned long blynkUpdateInterval = 2000;

// ---------------- PROTOTYPES ----------------
void beep();
void beepDubbel();
void beepLang();
void showTemp(String l1, String l2, int tijd = 1500);
bool inActief();
bool outActief();
String getStatusText();
void updateBlynkDisplay();
void handleStatus();
void handleAantal();
void handleButton();
void handleSimpleCounter();
void handleSound();

// ---------------- HELPERS ----------------
String getStatusText() {
  if (nietStoren) return "Niet storen";
  if (aantal == 0) return "Vrij";
  if (aantal < MAX_PERSONEN) return "Bezet";
  return "Vol";
}

void updateBlynkDisplay() {
  Blynk.virtualWrite(V4, aantal);
  Blynk.virtualWrite(V3, getStatusText());
}

// ---------------- BLYNK ----------------
BLYNK_WRITE(V0) {
  if (param.asInt() == 1) {
    nietStoren = true;
    geluidBlokkerenTot = millis() + 3000;
    lawaaiMelding = false;
    lawaaiLevel = 0;
    lawaaiTot = 0;
    showTemp("Niet storen", "Blynk AAN", 1500);
    updateBlynkDisplay();
  }
}

BLYNK_WRITE(V1) {
  if (param.asInt() == 1) {
    nietStoren = false;
    geluidBlokkerenTot = millis() + 3000;
    lawaaiMelding = false;
    lawaaiLevel = 0;
    lawaaiTot = 0;
    showTemp("Niet storen", "Blynk UIT", 1500);
    updateBlynkDisplay();
  }
}

BLYNK_WRITE(V2) {
  if (param.asInt() == 1) {
    aantal = 0;
    beepLang();
    showTemp("Blynk reset", "Aantal = 0", 1500);
    updateBlynkDisplay();
  }
}

// ---------------- BUZZER ----------------
void beep() {
  if (millis() - lastBeep > beepCooldown) {
    digitalWrite(BUZZER, HIGH);
    delay(80);
    digitalWrite(BUZZER, LOW);
    lastBeep = millis();
  }
}

void beepDubbel() {
  if (millis() - lastBeep > beepCooldown) {
    digitalWrite(BUZZER, HIGH);
    delay(70);
    digitalWrite(BUZZER, LOW);
    delay(70);
    digitalWrite(BUZZER, HIGH);
    delay(70);
    digitalWrite(BUZZER, LOW);
    lastBeep = millis();
  }
}

void beepLang() {
  if (millis() - lastBeep > beepCooldown) {
    digitalWrite(BUZZER, HIGH);
    delay(220);
    digitalWrite(BUZZER, LOW);
    lastBeep = millis();
  }
}

void showTemp(String l1, String l2, int tijd) {
  lijn1 = l1;
  lijn2 = l2;
  meldingTot = millis() + tijd;
}

bool inActief() {
  return digitalRead(IN_SENSOR) == LOW;
}

bool outActief() {
  return digitalRead(OUT_SENSOR) == LOW;
}

// ---------------- WEB ----------------
void handleStatus() {
  server.send(200, "text/plain", getStatusText());
}

void handleAantal() {
  server.send(200, "text/plain", String(aantal));
}

// ---------------- KNOP ----------------
void handleButton() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH && millis() - lastButtonTime > 300) {
    nietStoren = !nietStoren;
    lastButtonTime = millis();

    // 3 sec geen geluidsdetectie na knopdruk
    geluidBlokkerenTot = millis() + 3000;

    // reset lawaai status
    lawaaiMelding = false;
    lawaaiLevel = 0;
    lawaaiTot = 0;

    if (nietStoren) {
      showTemp("Niet storen", "Actief", 1500);
    } else {
      showTemp("Niet storen", "Uit", 1500);
    }

    updateBlynkDisplay();
  }

  lastButtonState = buttonState;
}

// ---------------- TELLER ----------------
void handleSimpleCounter() {
  unsigned long now = millis();

  bool inStatus = inActief();
  bool outStatus = outActief();

  if (inArmed && inStatus && (now - lastInTrigger > sensorCooldown)) {
    if (aantal < MAX_PERSONEN) {
      aantal++;
      Serial.print("BINNEN -> aantal = ");
      Serial.println(aantal);
      beep();
      showTemp("Welkom", "+1 persoon");
      updateBlynkDisplay();
    } else {
      beepLang();
      showTemp("Ruimte vol", "Geen plaats");
      updateBlynkDisplay();
    }

    lastInTrigger = now;
    inArmed = false;
  }

  if (outArmed && outStatus && (now - lastOutTrigger > sensorCooldown)) {
    if (aantal > 0) {
      aantal--;
      Serial.print("BUITEN -> aantal = ");
      Serial.println(aantal);
      beepDubbel();
      showTemp("Tot straks", "-1 persoon");
      updateBlynkDisplay();
    }

    lastOutTrigger = now;
    outArmed = false;
  }

  if (!inStatus) inArmed = true;
  if (!outStatus) outArmed = true;
}

// ---------------- LAWAAI ----------------
void handleSound() {
  if (millis() < geluidBlokkerenTot) {
    return;
  }

  int geluid = analogRead(SOUND_PIN);
  int verschil = abs(geluid - rustGeluid);

  Serial.print("Geluid: ");
  Serial.print(geluid);
  Serial.print(" | Rust: ");
  Serial.print(rustGeluid);
  Serial.print(" | Verschil: ");
  Serial.println(verschil);

  if (millis() - laatsteLawaaiTijd > lawaaiResetTijd) {
    lawaaiLevel = 0;
  }

  if (verschil > geluidDrempel && nietStoren && aantal > 0) {
    if (millis() - laatsteLawaaiTijd > 2000) {
      lawaaiLevel++;
      laatsteLawaaiTijd = millis();

      if (lawaaiLevel == 1) {
        lawaaiMelding = true;
        lawaaiTot = millis() + 2000;
        showTemp("Te luid", "Waarschuwing 1", 1200);
        beep();
        Serial.println("LAWAAI 1");
      }
      else if (lawaaiLevel == 2) {
        lawaaiMelding = true;
        lawaaiTot = millis() + 2500;
        showTemp("Te luid", "Laatste waars.", 1400);
        beepLang();
        Serial.println("LAWAAI 2");
      }
      else if (lawaaiLevel >= 3) {
        lawaaiMelding = true;
        lawaaiTot = millis() + 3000;
        showTemp("Te luid", "Rustig aub", 1500);
        beepLang();
        Serial.println("LAWAAI 3");
        lawaaiLevel = 0;
      }

      updateBlynkDisplay();
    }
  }

  if (millis() > lawaaiTot) {
    lawaaiMelding = false;
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(IN_SENSOR, INPUT_PULLUP);
  pinMode(OUT_SENSOR, INPUT_PULLUP);
  pinMode(SOUND_PIN, INPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(BUZZER, LOW);

  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Opstarten...");
  lcd.setCursor(0, 1);
  lcd.print("Geluid meten");

  // Rustgeluid meten bij opstart
  long som = 0;
  for (int i = 0; i < 100; i++) {
    som += analogRead(SOUND_PIN);
    delay(10);
  }
  rustGeluid = som / 100;

  Serial.print("Rustgeluid = ");
  Serial.println(rustGeluid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  server.on("/status", handleStatus);
  server.on("/aantal", handleAantal);
  server.begin();

  showTemp("Studieruimte", "Systeem start", 2000);
  updateBlynkDisplay();
}

// ---------------- LOOP ----------------
void loop() {
  Blynk.run();
  server.handleClient();

  handleButton();
  handleSound();
  handleSimpleCounter();

  if (millis() - lastBlynkUpdate > blynkUpdateInterval) {
    updateBlynkDisplay();
    lastBlynkUpdate = millis();
  }

  int vrij = MAX_PERSONEN - aantal;
  if (vrij < 0) vrij = 0;

  if (lawaaiMelding) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("Te luid        ");
    lcd.setCursor(0, 1);
    lcd.print("Rustig aub     ");
  }
  else if (millis() < meldingTot) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);

    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(lijn1);

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(lijn2);
  }
  else if (aantal == 0) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("Studieruimte   ");
    lcd.setCursor(0, 1);
    lcd.print("Vrij - ");
    lcd.print(vrij);
    lcd.print(" vrij   ");
  }
  else if (aantal < MAX_PERSONEN) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BLUE_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("Stille zone    ");
    lcd.setCursor(0, 1);
    lcd.print(aantal);
    lcd.print("/");
    lcd.print(MAX_PERSONEN);
    lcd.print(" aanwezig   ");
  }
  else {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("Ruimte vol     ");
    lcd.setCursor(0, 1);
    lcd.print("Kom later terug");
  }
}
