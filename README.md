# Slimme Studieruimte

## 📌 Project beschrijving
Dit project is een slimme studieruimte die automatisch detecteert hoeveel personen binnen en buiten gaan.

Het systeem gebruikt sensoren om het aantal personen te tellen en toont de status op een LCD scherm.

De ruimte kan 3 statussen hebben:
- Vrij
- Bezet
- Vol

Daarnaast is er een "Niet storen" functie met een drukknop en Blynk.

---

## ⚙️ Functionaliteiten
- Personen tellen (IN/OUT sensoren)
- LCD display met status
- RGB LED status:
  - Groen = Vrij
  - Geel = Bezet
  - Rood = Vol
- Geluidsdetectie (waarschuwing bij lawaai)
- Buzzer feedback
- Drukknop voor "Niet storen"
- Blynk app bediening

---

## 🔌 Hardware
- ESP32
- LCD 16x2
- 2x IR sensor (IN/OUT)
- Geluidsensor
- RGB LED
- Buzzer
- Drukknop
- Weerstanden
- Breadboard + wires

---

## 📊 Werking
Het systeem detecteert wanneer iemand binnenkomt of buiten gaat via sensoren.

Het aantal personen wordt aangepast en weergegeven op het LCD.

Bij te veel lawaai wordt een waarschuwing gegeven.

De gebruiker kan via een knop of app "Niet storen" activeren.

---

## 📸 Hardware foto's
Zie map `/hardware`

---

## 💻 Code
Zie map `/code`

---

## 📱 App (Blynk)
Via Blynk kan je:
- Status bekijken
- Niet storen activeren
- Reset doen

---

## 🎥 Demo video
Link: https://youtu.be/6bmW4w0Qr7U?feature=shared
