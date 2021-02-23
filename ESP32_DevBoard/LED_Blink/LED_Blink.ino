#define LED_PIN 16

void setup() {
  pinMode(LED_PIN, OUTPUT);     // Initialize the LED_PIN pin as an output
}

void loop() {
  digitalWrite(LED_PIN, HIGH);   // Turn the LED on 
  delay(1000);                   // Wait for a second
  digitalWrite(LED_PIN, LOW);    // Turn the LED off 
  delay(1000);                   // Wait again
}
