#include <Arduino.h>
#include <Stepper.h>
#include <Preferences.h> // For storing motor position

// Constants for stepper motor
const int stepsPerRevolution = 2048;  // Steps per revolution for 28BYJ-48
Stepper stepper(stepsPerRevolution, 40, 39, 37, 19); // IN1, IN2, IN3, IN4

// Pin assignments
const int pumpPin = 9;
const int mixValve1Pin = 41;
const int mixValve2Pin = 5;
const int mixValve3Pin = 42;
const int releaseValveN2Pin = 36;
const int releaseValveO2Pin = 15;
const int CO2PumpPin = 6;
const int O3GeneratorPin = 4;

// Preferences to store the motor position
Preferences preferences;
const char* motorPositionKey = "motorPosition";
long motorPosition = 0;  // Current position of the stepper motor

// PID control variables
float targetFlowRate = 0.0; // Target flow rate in SLM
float currentFlowRate = 0.0; // Current flow rate in SLM
// Add PID variables as needed

// Function prototypes
void setup();
void loop();
void injectGas(String gasType, float flowRate);
void controlValve(int valvePin, bool open);
void checkPressure();
void storeMotorPosition();
void restoreMotorPosition();
float readFlowInSLM(); // Dummy function for flow reading
float readPressure(String gasType); // Dummy function for pressure reading
void handleGasGeneration(String gasType);
void mixedGasInjection(float o2Percent, float n2Percent, float airPercent);

void setup() {
    Serial.begin(115200);
    preferences.begin("gasControl", false); // Start preferences
    restoreMotorPosition(); // Restore the last motor position
    // Set pin modes
    pinMode(pumpPin, OUTPUT);
    pinMode(mixValve1Pin, OUTPUT);
    pinMode(mixValve2Pin, OUTPUT);
    pinMode(mixValve3Pin, OUTPUT);
    pinMode(releaseValveN2Pin, OUTPUT);
    pinMode(releaseValveO2Pin, OUTPUT);
    pinMode(CO2PumpPin, OUTPUT);
    pinMode(O3GeneratorPin, OUTPUT);
    // Initialize any other required components
}

void loop() {
    checkPressure(); // Monitor pressures
    // Call functions based on scenarios here
    // e.g., injectGas("O2", targetFlowRate);
    // Use non-blocking timing strategies like millis() for timing
}

void injectGas(String gasType, float flowRate) {
    // Set target flow rate
    targetFlowRate = flowRate;
    // Control the stepper motor and valves based on the gas type
    // Implement PID control here
}

void controlValve(int valvePin, bool open) {
    digitalWrite(valvePin, open ? HIGH : LOW); // Activate or deactivate the valve
}

void checkPressure() {
    float o2Pressure = readPressure("O2");
    float n2Pressure = readPressure("N2");
    // Add pressure checks and control logic
    if (o2Pressure > 5.0) {
        controlValve(releaseValveO2Pin, true);
    } else if (o2Pressure < 2.0) {
        handleGasGeneration("O2");
    }
    if (n2Pressure > 5.0) {
        controlValve(releaseValveN2Pin, true);
    } else if (n2Pressure < 2.0) {
        handleGasGeneration("N2");
    }
}

void storeMotorPosition() {
    preferences.putLong(motorPositionKey, motorPosition);
}

void restoreMotorPosition() {
    motorPosition = preferences.getLong(motorPositionKey, 0);
    stepper.setCurrentPosition(motorPosition); // Set stepper position
}

void mixedGasInjection(float o2Percent, float n2Percent, float airPercent) {
    // Calculate the total time for each gas injection based on percentages
    // Adjust timing using millis() to avoid blocking
}
