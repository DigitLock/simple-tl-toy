void setup() {
  Serial.begin(115200);
  delay(2000);  // даём время на подключение USB CDC
  Serial.println(F("Serial OK!"));
  pinMode(PC13, OUTPUT);
}

void loop() {
  Serial.println(F("blink!"));
  digitalWrite(PC13, HIGH);
  delay(1000);
  digitalWrite(PC13, LOW);
  delay(1000);
}