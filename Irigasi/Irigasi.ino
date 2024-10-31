#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PID_v1.h>
  
// Pin definitions
const int sensorPin = A0; // Sensor kelembaban tanah
const int potPin = A1; // Potentiometer untuk PID tuning
const int enaPin = 3; // ENA pada L298N
const int in1Pin = 4; // IN1 pada L298N
const int in2Pin = 5; // IN2 pada L298N

// PID parameters
double setpoint, input, output;
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);
 
// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
// Timing variables
unsigned long previousMillis = 0;
const long interval = 5000; // Interval for display update
bool displaySetpoint = true; // Toggle between setpoint and PID values

void setup() {
pinMode(sensorPin, INPUT);
pinMode(potPin, INPUT);
pinMode(enaPin, OUTPUT);
pinMode(in1Pin, OUTPUT);
pinMode(in2Pin, OUTPUT);
 
// Initialize LCD
lcd.init();
lcd.backlight();
 
// Initialize PID
setpoint = 700; // Ambang batas kelembaban
myPID.SetMode(AUTOMATIC);
myPID.SetOutputLimits(0, 255);

digitalWrite(in1Pin, LOW);
digitalWrite(in2Pin, LOW);
}
 
void loop() {
// Read sensor value
int sensorValue = analogRead(sensorPin);
input = sensorValue;
 
// Read potentiometer for PID tuning
int potValue = analogRead(potPin);
setpoint = map(potValue, 0, 1023, 500, 800);

// Compute PID output
myPID.Compute();
 
// Update display every interval
unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= interval) {
previousMillis = currentMillis;
 
if (displaySetpoint) {
// Display setpoint and sensor value
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Kelembaban: ");
lcd.print(sensorValue);
lcd.setCursor(0, 1);
lcd.print("Setpoint: ");
lcd.print(setpoint);
} else {
// Display PID values
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Input: ");
lcd.print(input);
lcd.setCursor(0, 1);
lcd.print("Output: ");
lcd.print(output);
}
 
displaySetpoint = !displaySetpoint; // Toggle display
}
 
// Control the motor (pump)
if (sensorValue > 400 && sensorValue < 900) {
if (sensorValue > setpoint) {
// Jika kelembaban di bawah setpoint, pompa menyala
digitalWrite(in1Pin, HIGH);
digitalWrite(in2Pin, LOW);
analogWrite(enaPin, output); // Kontrol kecepatan pompa dengan output PID
} else {
// Jika kelembaban di atas setpoint, pompa mati
digitalWrite(in1Pin, LOW);
digitalWrite(in2Pin, LOW);
analogWrite(enaPin, 0); // Matikan pompa
}

} else {
// Jika kelembaban di luar rentang, pompa mati
digitalWrite(in1Pin, LOW);
digitalWrite(in2Pin, LOW);
analogWrite(enaPin, 0); // Matikan pompa
}

delay(1000); // Baca sensor setiap 1 detik
}