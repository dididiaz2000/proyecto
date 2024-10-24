# PROYECTO 
## Comienzo
He comenzado con el proyecto y compila bien. Lo que es la plataforma, como tal, este commit es el bueno por ahora.

### platformio.ini
Basándome en el código JSON, lo he convertido en el platformio.ini según las especificaciones que indicaba. Para que funcionen todas las bibliotecas que necesitaba, he añadido una línea más en la configuración:

    $ lib_deps = Stepper

Por facilidad, he eliminado el segundo framework de "espidf". Corrigiendo estos errores, ya funciona el platformio.ini.

## Proyecto
He creado un nuevo proyecto basandome en el primero y en otros proyectos.

### platformio.ini:
Para que funcionen las bibliotecas que necesitaba, he añadido una librería más en la configuración:

    $ lib_deps = Stepper, br3ttb/PID @ ^1.2.0

### proyecto.cpp 
1. Librerías
2. Definimos los pines según la tabla
    IN1-IN4: pines de las boninas del motor.
3. Configuramos el motor paso a paso (en nuestro caso 28BYJ-48, 2048 pasos por revolución)(1)
    stepper:  objeto que controla el motor
    stepsPerRevolution: número de pasos por revolución
4. Utilizamos un controlador PID para ajustar el valor de salida, de entrada y un valor deseado.(2)
    - Variables:
        - setpoint: valor a alcanzar.
        - input: valor que mide el sistema.
        - output: valor que el PID va a calcular para aplicar una corrección (en nuestro caso, controlará una válvula).
    - Ganancias:
        - Kp(proporcional): término que corrige la parte proporcional al error. Mientras más grande sea el error (diferencia entre setpoint e input), más grande será la corrección.
        - Ki(integral): este término suma los errores pasados. Se usa para eliminar errores persistentes.
        - Kd(derivativo): este término tiene en cuenta la velocidad del cambio de error. Ayuda a predecir el futuro error y hacer ajustes más suaves.
    - PID:
        Creamos un objeto de PID y lo configuramos con los parámetros ya definidos. El último "DIRECT" indica que el actuador debe actuar directamente sobre la salida para corregir el error. Si fuera "REVERSE", significaría que el sistema tendría un comportamiento inverso.
5. Iniciamos las variables de control para  el flujo y posicion del motor.
    - stepperPosition: almacena la posición actual del motor paso a paso en términos de pasos. Está en 0, lo que sugiere que el motor está en su posición de referencia.v
    - targetFlowRate: almacena la tasa del flujo objetivo que se desea alcanzar. Representa cuántos litros por minuto de gas se desea inyectar.
    - currentFlowRate: almacena la tasa de flujo de gas actual medida con el sensor. Se compara con targetFlowRate para determinar si se necesita ajustar el sistema.
    - valveState: almacena el estado actual de una válvula. Low significa que está cerrada.
6. void setup()
    Es la función que se ejecuta cuando se inicia el microcontrolador.
    - Inicializamos los pines: 
        Todos se configuran como salidas. Significa que estos pines se usarán para enviar señales a las válvulas o al generador de O3, para abrirlas o cerrarlas.
    - Inicializamos el Motor Paso a Paso:
        Establece la velocidad del motor paso a paso en revoluciones por minuto, en nuestro caso en 15 rpm.
    - Configuramos el PID:
        - SetMode(AUTOMATIC): Modo automático. Esto significa que el PID comenzará a calcular automáticamente las salidas en función de input y setpoint que se le proporcionen más adelante en el código.
        - SetOutputLimits(-100,100):  Limita el rango de salida del PID entre -100 y 100. Es importante porque la salida se utilizará para determinar cuántos pasos debe mover el motor. Limitarlo ayuda a evitar movimientos excesivos o erráticos del motor.
    - Recuperar la posicion del motor:
    Una función que simula la recuperación de la posición anterior del motor desde una memoria no volátil.
7. void loop()
    - Con la función continuousO2Injection() se gestiona la inyección continua de oxígeno. Más adelante se explica la función.  
    Análogamente con el resto.
    - Con la función continuousMixedGasInjection() se inyecta una mezcla de gases en las proporciones especificadas. Lo mismo, más adelante se explica mejor la función.
    - La tasa de flujo de O2 (10 SLM) garantiza que haya suficiente oxígeno para crear O3 a la cantidad requerida para la aplicación.
    - La cantidad de O3 inyectado (7 SLM) está vinculada a la demanda de ozono del sistema, y las proporciones se ajustan en consecuencia para equilibrar el uso de oxígeno y la producción de ozono.
    - La cantidad de N2 inyectado (5 atm). Si la presión del tanque supera 5 atm, se debe abrir la válvula de liberación hasta que la presión baje a un nivel seguro. Esto sugiere que 5 atm es el límite máximo que el sistema puede manejar sin riesgos de sobrepresión.
    - Como las especificaciones dicen que la presión tiene que estar entre 2 atm y 5 atm, las tasas se establecen de manera que no sobrepasen esos límites de presión. Si la presión cae por debajo de 2 atm, se activa la bandera de generación de gas. 
    - En el caso de la inyección de gases mixtos, los valores como 3 SLM de O2, 4 SLM de N2 y 2 SLM de aire han sido calculados para mantener una mezcla equilibrada. Está diseñado para que el gas inyectado sea homogéneo y seguro. Además, las tasas han sido ajustadas para optimizar el rendimiento del PID, asegurando que las válvulas y el motor respondan de manera eficiente.
8. readFlowinSLM()  
    Simula la lectura del flujo de gas
    - Agregar fluctuaciones aleatorias. Generar una pequeña variación aleatoria (+/- 5 unidades) para simular el ruido del sensor.
        ```cpp
        int readFlowinSLM() {
            int simulatedFlow = currentFlowRate + random(-5, 5); 
            return simulatedFlow;
        }
        ```
    - Simular un flujo progresivo o dependiente del tiempo, imitando un sistema donde el flujo cambia con el tiempo.
        ```cpp
        int readFlowinSLM() {
            currentFlowRate += random(-1, 2);
            if (currentFlowRate < 0) currentFlowRate = 0;
            return currentFlowRate;
        }
        ```
    - Simular el comportamiento de un sensor real basado en objetos, se puede implementar una lógica que intente aproximar el flujo simulado.
        ```cpp
        int readFlowinSLM() {
            if (currentFlowRate < targetFlowRate){
                currentFlowRate++;
            } else if (currentFlowRate > targetFlowRate) {
                currentFlowRate--;
            }
            return currentFlowRate;
        }
        ```
    - Simulación completa con variabilidad realista, ideas para simular un comportamiento más completo, incluyendo fluctuaciones, ajustes dinámicos y límites de flujo.
        ```cpp
        int readFlowinSLM() {
            int fluctuation = random(-3, 3); 
            if (currentFlowRate < targetFlowRate) {
                currentFlowRate += 1 + fluctuation; 
            } else if (currentFlowRate > targetFlowRate) {
                currentFlowRate -= 1 + fluctuation; 
            }
            if (currentFlowRate < 0) currentFlowRate = 0;
            if (currentFlowRate > targetFlowRate + 10) currentFlowRate = targetFlowRate + 10;
            return currentFlowRate;
        }
        ```
9. storeStepperPosition()  
    Simula el almacenamiento del motor paso a paso, que normalmente estaría en la memoria no volátil. En este paso, tenemos que añadir algunas cosas:
    - Incluir las librerías necesarias para usar la memoria no volátil (NVS):
        ```cpp
        #include <nvs.h>
        #include <nvs_flash.h>
        ```
    - Inicializar NVS en el setup():
        ```cpp
        void setup() {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
    
        // Luego inicializas el motor, el PID, etc.
        }
        ```
    - Implementar la función en el loop():
        ```cpp
        void loop() {
            // Código del sistema...
            // Mover motor según sea necesario
            // Actualizar la posición y guardarla en NVS
            storeStepperPosition();
        }
        ```
    - Implementamos la función storeStepperPosition(), guardará la posición del motor en la NVS:
        ```cpp
        void storeStepperPosition() {
            // Abrir el handle de NVS
            nvs_handle_t nvs_handle;
            esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
            if (err != ESP_OK) {
                Serial.println("Error al abrir NVS");
                return;
            }

            // Guardar la posición del motor (entero)
            err = nvs_set_i32(nvs_handle, "stepperPos", stepperPosition);
            if (err != ESP_OK) {
                Serial.println("Error al guardar en NVS");
            }

            // Guardar los cambios
            err = nvs_commit(nvs_handle);
            if (err != ESP_OK) {
                Serial.println("Error al confirmar cambios en NVS");
            }

            // Cerrar el handle de NVS
            nvs_close(nvs_handle);
        }
        ```
10. retrieveStepperPosition()  
    Función para leer la última posición almacenada al reiniciar el sistema:
    ```cpp
    int retrieveStepperPosition() {
        nvs_handle_t nvs_handle;
        int32_t storedStepperPosition = 0;

        // Abrir el handle de NVS
        esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
        if (err == ESP_OK) {
            // Leer la posición almacenada
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
    ```
11. moveToPosition()  
    Mueve el motor paso a paso a una posición específica, ajustando con PID en función de la tasa de flujo medida.
    - Primero controlamos los límites del motor, nos aseguramos que la posición no baje de 0 y no supere los 2048 pasos.
    - Usamos el PID para regular el flujo de gases. El PID ajusta la posición del motor en función de la tasa de flujo medida y el objetivo(input+setpoint).
12. moveStepper()  
    Controla el motor paso a paso moviendo la cantidad de pasos necesaria.
13. controlFlow()  
    Controla el flujo con el PID, ajustando la velocidad del motor según la desviación entre setpoint y currentFlowRate.
14. smoothFlowRate()  
    Aplica un filtro de suavizado a las lecturas del sensor de flujo. Los sensores de flujo pueden ser ruidosos. Si la tasa de flujo fluctúa mucho debido al ruido del sensor, por ello se implementa un filtro de suavizado en las lecturas del sensor para obtener una medida mas estable.
    - Se guardan las ultimas lecturas. El filtro mantiene un historial de lecturas para calcular el promedio. Cuanto más alto es el número mas lento se vuelve el filtro.
    - Se agrega la nueva lectura. Llega la nueva lectura y esta reemplaza a una de las lecturas anteriores.
    - Calcula el promedio. 
    - Devuelve el valor suavizado. Garantiza que los pequeños cambios en la lectura no afecten la salida del sistema de control.
15. activateValve() y deactivateValve()  
    Son las funciones para activar y desactivar las valvulas de acuerdo con el input.
    - Conectadas a los pines del microcontrolador ESP32. Las valvulas estan controladas por MOSFETs, estos activan y desactivan mediante una señal de control un pin digital.
    - Cada valvula tiene asignado un pin específico.
    - HIGH para abrir y LOW para cerrar.
16. injectGas()
    Esta función, la verdad, es para hacer mas escalable el codigo, encapsula todo el proceso de inyección de gas en una unica función. Utiliza las funciones ya mencionadas para inyectar el gas.
17. calculateStepsForFlowRate()
    Diseñada para convertir un flujo de gas deseado en la cantidad e pasos que debe moverse el motor paso a paso para alcanzar ese flujo.
    - Define los pasos por revolución.
    - Define el flujo maximo.
    - Se calculan los pasos por SLM.
18. continuousInjection()
    Son las funciones para los escenarios de inyección de gas.
    - O2 inyecta oxigeno aun flujo específico, maneja la apertura de la valvula, control del flujo y el cierre de la valvula.
    - N2 análogo al de O2.
    - O3 esta función es un poco mas compleja, requiere activar el generador de ozono antes de inyectar oxigeno. Al abrir el generador de ozono, se asegura que cuando se inyecta oxigeno, se produce ozono, dado que el generador convierte O2 en O3.
19. continuousMixedGasInjection()
    Diseñada para inyectar una mezcla de gases (O2, N2 y aire filtrado).
    - Los parametros de entrada son las tasas de flujo objetivo que se inyectará.
    - Las valvulas se abren de manera secuencial sin bloquear el sistema, se busca mantener la funcionalidad y la reactividad del sistema mientras se manejan los flujos de gas.
    - La función llama tres veces a la funcion para inyectar cada gas:O2, N2 y aire filtrado.

### default_16MB.csv
    Describe como se va a particionar la memoria flash del ESP32-S3.
    ```default_16MB.csv
    # Name,   Type, SubType, Offset,  Size, Flags
    nvs,      data, nvs,     0x9000,  0x5000,
    otadata,  data, ota,     0xe000,  0x2000,
    app0,     app,  ota_0,   0x10000, 0x310000,
    app1,     app,  ota_1,   0x320000, 0x310000,
    spiffs,   data, spiffs,  0x630000,0x1d0000,
    ```
## Modificaciones para usar el config.json
Se añaden las siguientes modificaciones para usar un archivo de configuración config.json proporcionado.

### Entorno virtual o no
En caso de que no queramos usar el entorno virtual:
    $ pio run
Para compilar el proyecto.

Como a veces no deja utilizar todos los comandos de platformio en la terminal podemos crear un entorno virtual.
1. Creamos un nuevo entorno virtual
    $ python3 -m venv venv
2. Activamos el entorno virtual
    $ source venv/bin/activate
3. Desactivamos el entorno virtual
    $ deactivate

### load_config.py
Para poder usar el config.json primero tenemos que configurar un archivo llamado load_config.py en python. Lo que hace es cargar un archivo de configuración en formato JSON.

### platformio.ini
Para poder usar el load_config.py debemos añadir la siguiente linea:
    $ extra_scripts = load_config.py

## Apuntes
(1) Esto define la precisión del control, un valor de pasos más alto significa un movimiento más fino.  
(2) Un controlador PID es un algoritmo de control utilizado para mantener una variable (en nuestro caso presión) lo mas cerca posible de un valor deseado (setpoint) mediante ajustes automaticos.