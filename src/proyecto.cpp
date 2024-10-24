#include <Arduino.h>
#include <Stepper.h>
#include <PID_v1.h>
#include <nvs.h>
#include <nvs_flash.h>

#define IN1 40
#define IN2 39
#define IN3 37
#define IN4 19
#define O3_GEN_PIN 4
#define N2_RELEASE_PIN 36
#define O2_RELEASE_PIN 15
#define MixValve1 5

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

// Declaraciones de funciones
void continuousO2Injection(int targetFlow);
void continuousN2Injection(int targetFlow);
void continuousO3Injection(int targetFlow);
void continuousMixedGasInjection(int targetFlowO2, int targetFlowN2, int targetFlowAir);
void storeStepperPosition();
int retrieveStepperPosition();
int smoothFlowRate(int newFlowRate);
void controlFlow();
int calculateStepsForFlowRate(float flowRate);
void moveStepper(int steps);

void setup() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inicializar pines
    pinMode(O3_GEN_PIN, OUTPUT);
    pinMode(N2_RELEASE_PIN, OUTPUT);
    pinMode(O2_RELEASE_PIN, OUTPUT);
    pinMode(MixValve1, OUTPUT);

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

    // Actualizar la posición y guardarla en NVS
    storeStepperPosition();
}

// Simular la lectura del flujo en SLM
int readFlowinSLM() {
    if (currentFlowRate < targetFlowRate){
        currentFlowRate++;
    } else if (currentFlowRate > targetFlowRate) {
        currentFlowRate--;
    }
    return currentFlowRate;
}

// Función para guardar y restaurar la posición del motor (simulación de EEPROM)
void storeStepperPosition() {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        Serial.println("Error al abrir NVS");
        return;
    }
    err = nvs_set_i32(nvs_handle, "stepperPos", stepperPosition);
    if (err != ESP_OK) {
        Serial.println("Error al guardar en NVS");
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        Serial.println("Error al confirmar cambios en NVS");
    }
    nvs_close(nvs_handle);
}

int retrieveStepperPosition() {
    nvs_handle_t nvs_handle;
    int32_t storedStepperPosition = 0;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        err = nvs_get_i32(nvs_handle, "stepperPos", &storedStepperPosition);
        if (err == ESP_OK) {
            Serial.println("Posición recuperada de NVS");
        } else {
            Serial.println("No se encontró la posición en NVS");
        }
        nvs_close(nvs_handle);
    } else {
        Serial.println("Error al abrir NVS para leer");
    }
    return storedStepperPosition;
}

// Función para mover el motor a una posición específica
void moveToPosition(int targetPosition) {
    if (targetPosition < 0) {
        targetPosition = 0;
    } else if (targetPosition > 2048) {
        targetPosition = 2048;
    }
    int stepsToMove = targetPosition - stepperPosition;
    stepper.step(stepsToMove);
    stepperPosition = targetPosition;
    storeStepperPosition();
}

void moveStepper(int steps) {
    // Mueve el motor la cantidad de pasos especificada
    if (steps > 0) {
        stepper.step(steps); // Mover hacia adelante
    } else {
        stepper.step(-steps); // Mover hacia atrás
    }
}

// Función para manejar el control PID del flujo
void controlFlow() {
    currentFlowRate = readFlowinSLM();
    input = currentFlowRate;
    setpoint = targetFlowRate;
    
    // Actualizar el PID
    myPID.Compute();

    // Calcular el error entre el setpoint y el flujo actual
    int error = abs(setpoint - currentFlowRate);

    // Ajustar la velocidad del motor según el error
    if (error > 50) {
        stepper.setSpeed(30);  // Velocidad alta para grandes correcciones
    } else {
        stepper.setSpeed(15);  // Velocidad baja para ajustes finos
    }

    // Filtro de suavizado
    currentFlowRate = smoothFlowRate(readFlowinSLM());
    
    // Mover el motor según el resultado del PID
    int motorSteps = (int)output;
    moveToPosition(stepperPosition + motorSteps);
}

// Filtro de suavizado para la señal de flujo
int smoothFlowRate(int newFlowRate) {
    static int flowRates[10];  // Guardar las últimas 10 lecturas
    static int index = 0;
    int total = 0;

    flowRates[index] = newFlowRate;  // Guardar la nueva lectura
    index = (index + 1) % 10;  // Ciclar el índice

    // Promediar las lecturas
    for (int i = 0; i < 10; i++) {
        total += flowRates[i];
    }

    return total / 10;  // Devolver el promedio
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

int calculateStepsForFlowRate(float flowRate) {
    int stepsPerRevolution = 2048; // Número de pasos por revolución
    float maxFlowRate = 10.0; // Flujo máximo en SLM
    float stepsPerSLM = stepsPerRevolution / maxFlowRate; // Calcular pasos por SLM
    return flowRate * stepsPerSLM;
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
