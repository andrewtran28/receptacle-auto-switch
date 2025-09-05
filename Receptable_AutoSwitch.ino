#define CURRENT_SENSOR_PIN 4      // GPIO 4 (labeled "D4" on some boards)
#define RELAY_PIN 25              // GPIO 25
#define RELAY_LOGIC LOW           // Set to LOW if your relay is activated by a ground signal. 
                                  // Set to HIGH if your relay is activated by a 5V signal. 
                                  // If your relay is active at bootup, and turns off when CURRENT_THRESHOLD is met, 
                                  // then try changing this setting to the opposite.  

#define CURRENT_THRESHOLD 3300    // The analog value above which the relay shall be triggered
#define CURRENT_SAMPLE_PERIOD 500 // Time (ms) to sample the current reading
#define RELAY_STARTUP_DELAY 1000  // Time (ms) to delay before activating relay after tool has started
#define RELAY_SHUTOFF_DELAY 3000  // Time (ms) to delay shutting off the relay after the tool has been shutoff
#define STARTUP_GRACE_PERIOD 1000 // Time (ms) to ignore current sensor (forces high threshold at boot)

int analogValue = 0;              // Stores ADC values read in from the current sensor
unsigned long stopwatch = 0;      // Used to keep track of elapsed time
boolean relayPinState = HIGH;     // Stores the current state of the relay pin
unsigned long startupTime = 0;    // Stores the time the device was powered on

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);     // Configure relay pin early

  digitalWrite(RELAY_PIN, !RELAY_LOGIC); // Force relay OFF at boot
  startupTime = millis();         // Record the time the board started
}

void loop() {
  analogValue = 0;                // Initialize analogValue to a known state
  stopwatch = millis();           // Store a snapshot of the current time since the program started executing

  // Collect the max ADC value from the current sensor for the predetermined sample period
  while(millis() - stopwatch < CURRENT_SAMPLE_PERIOD) { 
    analogValue = max(analogValue, (int)analogRead(CURRENT_SENSOR_PIN));
    Serial.println(analogValue);
  }

  // During startup grace period, ignore the current sensor by setting the threshold very high
  int effectiveThreshold = CURRENT_THRESHOLD;
  if (millis() - startupTime < STARTUP_GRACE_PERIOD) {
    effectiveThreshold = 9999;   // Arbitrary high value to ensure relay does not trigger
  }

  // If the max ADC value from the current sensor exceeds the effective threshold, activate the relay
  if(analogValue > effectiveThreshold) {
    delay(RELAY_STARTUP_DELAY);
    relayPinState = RELAY_LOGIC;
  }
  // Otherwise if the relay was just activated, wait for the predetermined shutoff delay and set the state to HIGH
  else {
    if(relayPinState == RELAY_LOGIC) {
      delay(RELAY_SHUTOFF_DELAY);
      relayPinState = !relayPinState;
    }
  }

  digitalWrite(RELAY_PIN, relayPinState); // Write the RELAY_PIN to the correct state
}
