#define NUM_BUTTONS 7
#define COMMON_PIN 2

bool started = false;

String COMMANDS[2] = { "off:", "on:" };

// Pinout definition
int8_t buttons[NUM_BUTTONS] = { 3,4,5,6,7,8,9 };
int8_t leds[NUM_BUTTONS] = { A3,A2,A1,A0,10,11,12 };

// Global effect last known state
bool states[NUM_BUTTONS];

volatile int8_t num_effect = 0;
volatile String command = "";

volatile int num_recvd = -1;
volatile bool state_recv = false;

volatile unsigned long lastFire = 0;

void pressInterrupt() {
    configureDistinct(); // Setup for testing invdividual buttons

    for (int i = 0; i < NUM_BUTTONS; i++) {
      if (!digitalRead(buttons[i])) {
        press(i);  
      }  
    }
    configureCommon();
}

void configureCommon() {
  pinMode(COMMON_PIN, INPUT_PULLUP);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i], OUTPUT);
    digitalWrite(buttons[i], LOW);  
  }
}

void configureDistinct() {
  pinMode(COMMON_PIN, OUTPUT);
  digitalWrite(COMMON_PIN, LOW);

    for (int i = 0; i < NUM_BUTTONS; i++) {
      pinMode(buttons[i], INPUT_PULLUP);  
    }
}

void press(int i) { // Press handler
  num_recvd = i;
}

void setup() {
    int8_t i;
    Serial.begin(115200);

    /* Pin setup */
    for (i = 0; i < NUM_BUTTONS; i++) {
        states[i] = false;
        // Pin modes setup
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
        states[i] = false;
    }
    configureCommon(); // Setup for interrupt
    attachInterrupt(digitalPinToInterrupt(COMMON_PIN), pressInterrupt, FALLING);
    /* End of pin setup */
    /* Startup animation */
    for (i = 0; i < NUM_BUTTONS / 2 + 1; i++) {
        digitalWrite(leds[i], HIGH);
        digitalWrite(leds[NUM_BUTTONS - 1 - i], HIGH);
        delay(150);
        digitalWrite(leds[i], LOW);
        digitalWrite(leds[NUM_BUTTONS - 1 - i], LOW);
    }
    /* End of startup animation */

    // Wait for START command
    String line;
    while (!started) {
        if (Serial.available() > 0 ) {
            line = Serial.readString();
            line.trim();
            if (line == "START") {
                started = true;
            }
        }
    }
    Serial.println("OK");

    // Serial connected and start message received
    /* Double blink animation on connection */
    for (i = 0; i < NUM_BUTTONS; i++) {
        digitalWrite(leds[i], HIGH);
    }
    delay(50);
    for (i = 0; i < NUM_BUTTONS; i++) {
        digitalWrite(leds[i], LOW);
    }
    delay(50);
    for (i = 0; i < NUM_BUTTONS; i++) {
        digitalWrite(leds[i], HIGH);
    }
    delay(50);
    for (i = 0; i < NUM_BUTTONS; i++) {
        digitalWrite(leds[i], LOW);
    }
    /* End of double blink animation */
}

void loop() {
    if (num_recvd > -1) {
        states[num_recvd] = !states[num_recvd];
        Serial.println(COMMANDS[(states[num_recvd]) ? 1 : 0] + String(num_recvd));
        digitalWrite(leds[num_recvd], (states[num_recvd]) ? HIGH : LOW);
        num_recvd = -1;
    }
    // Read from serial for updates from the pedalboard
    if (Serial.available()) {
        command = Serial.readStringUntil(':');
        command.trim();
        num_effect = Serial.parseInt();
        if ((command == "on" || command == "off") && num_effect < NUM_BUTTONS) {
            digitalWrite(leds[num_effect], (command == "on") ? HIGH : LOW);
            states[num_effect] = (command == "on") ? true : false;
        }
    }
}
