#include <Arduino.h>
#include <Stepper.h>
#include <PID_v1.h>

#define IN1 40
#define IN2 39
#define IN3 37
#define IN4 19
#define O3_GEN_PIN 4
#define N2_RELEASE_PIN 36
#define O2_RELEASE_PIN 15

const int stepsPerRevolution = 2048;
Stepper stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

// Variables del PID
double setpoint, input, output;
double Kp = 2.0, Ki = 5.0, Kd = 1.0;
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Variables para flujo y posición del motor
int stepperPosition = 0;
int targetFlowRate = 0;
int currentFlowRate = 0; // Simulación del flujo actual
int valveState = LOW;

void setup() {
    // Inicializar pines
    pinMode(O3_GEN_PIN, OUTPUT);
    pinMode(N2_RELEASE_PIN, OUTPUT);
    pinMode(O2_RELEASE_PIN, OUTPUT);

    // Inicializar el motor paso a paso
    stepper.setSpeed(15);  // Configurar la velocidad del motor en RPM

    // Configurar el PID
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(-100, 100); // Limitar el rango de salida del PID

    // Recuperar la posición del motor desde la memoria no volátil
    stepperPosition = retrieveStepperPosition();
}

void loop() {
    // Simulación de la lógica principal
    continuousO2Injection(10); // Inyectar O2 a 10 SLM
    continuousN2Injection(5);  // Inyectar N2 a 5 SLM
    continuousO3Injection(7);  // Inyectar O3 a 7 SLM
    continuousMixedGasInjection(3, 4, 2);  // Inyección de mezcla
}

// Simular la lectura del flujo en SLM
int readFlowinSLM() {
    // Función de simulación de lectura de flujo
    return currentFlowRate; // Aquí devolveríamos la lectura real del sensor
}

// Función para guardar y restaurar la posición del motor (simulación de EEPROM)
void storeStepperPosition() {
    // Guardar la posición en memoria no volátil
    // En una implementación real usaríamos la memoria no volátil (NVS)
}

int retrieveStepperPosition() {
    // Restaurar la posición desde la memoria no volátil
    return stepperPosition; // Simulación, devolveríamos el valor almacenado
}

// Función para mover el motor a una posición específica
void moveToPosition(int targetPosition) {
    int stepsToMove = targetPosition - stepperPosition;
    stepper.step(stepsToMove);
    stepperPosition = targetPosition;
    storeStepperPosition();
}

// Función para manejar el control PID del flujo
void controlFlow() {
    currentFlowRate = readFlowinSLM();
    input = currentFlowRate;
    setpoint = targetFlowRate;
    
    // Actualizar el PID
    myPID.Compute();
    
    // Mover el motor según el resultado del PID
    int motorSteps = (int)output;
    moveToPosition(stepperPosition + motorSteps);
}

// Función para activar las válvulas de acuerdo con la entrada
void activateValve(int pin) {
    digitalWrite(pin, HIGH);
}

void deactivateValve(int pin) {
    digitalWrite(pin, LOW);
}

// Simulación del flujo de gases en función de las válvulas
void injectGas(int valvePin, int targetFlow) {
    targetFlowRate = targetFlow;
    activateValve(valvePin);
    controlFlow();
    deactivateValve(valvePin);
}

// Funciones para escenarios de inyección de gases
void continuousO2Injection(int targetFlow) {
    injectGas(O2_RELEASE_PIN, targetFlow);
}

void continuousN2Injection(int targetFlow) {
    injectGas(N2_RELEASE_PIN, targetFlow);
}

void continuousO3Injection(int targetFlow) {
    activateValve(O3_GEN_PIN);
    injectGas(O2_RELEASE_PIN, targetFlow);
    deactivateValve(O3_GEN_PIN); // Apagar el generador de O3 al finalizar
}

void continuousMixedGasInjection(int targetFlowO2, int targetFlowN2, int targetFlowAir) {
    // Aquí ajustamos las proporciones y abrimos las válvulas de manera secuencial sin bloquear el sistema
    injectGas(O2_RELEASE_PIN, targetFlowO2);
    injectGas(N2_RELEASE_PIN, targetFlowN2);
    // Asumiendo que tenemos una válvula para aire filtrado (pseudocódigo)
    injectGas(MixValve1, targetFlowAir);
}
