# PROYECTO 
## Comienzo
He empezado con el proyecto y compila bien, lo que es la plataforma como tal, este commit es el bueno por ahora. 

### platformio.ini
Basandome en el codigo json, lo he convertido en el platformio.io segun las especificaciones que ponia.  
Para que funcionen todas las bibliotecas que necesitaba he añadido una linea de codigo mas en la configuración:

    $ lib_deps = Stepper

Tambien he tenido que cambiar una linea:

    $ upload_flash_size = 16

por: 

    $ board_flash_size = 16

Por facilidad he aliminado el segundo framework de "espidf".  
En el sdkconfig era encesario cambiar el "CONFIG_FREERTOS_HZ" de 100 a 1000.

Corrigiendo estos errores ya funciona el 'platformio.ini'.

### main.cpp:
- He añadido los includes necesarios.
- El codigo tiene muchas funciones que en principio deberian funcionar.
- El primer intento de compilacion lo deberia hacer el miercoles si todo va bien.

## Proyecto
He creado un nuevo proyecto basandome en el primero y en otros proyectos.

### platformio.ini:
Para que funcionen las bibliotecas que necesitaba he añadido una libreria mas en la configuración:

    $ lib_deps = Stepper, br3ttb/PID @ ^1.2.0

### proyecto.cpp 
1. Librerias
2. Definimos los pines segun la tabla
    IN1-IN4: pines de las boninas del motor.
3. Configuramos el motor paso a paso (en nuestro caso 28BYJ-48, 2048 pasos por revolución)(1)
    stepper:  objeto que controla el motor
    stepsPerRevolution: numero de pasos por revolucion
4. Utilizamos un controlador PID para ajustar el valor de salida, de entrada y un valor deseado.(2)
    - Variables:
        - setpoint: valor a alcanzar.
        - input: valor que mide el sistema.
        - output: valor que el PID va a calcular para aplicar una correcion (en nuestro caso controlara una valvula).
    - Ganancias:
        - Kp(proporcional): termino  que corrige la parte proporcinal al error. Mientras mas grande sea el error (diferencia entre setpoint e input), mas grande sera la correccion.
        - Ki(integral): este termino suma los errores pasados. Se usa para eliminar errores persistentes.
        - Kd(derivativo): este termino tiene en cuenta la  velocidad del cambio de error. Ayuda a predecir el futuro error y hacer ajustes mas suaves.
    - PID:
        Creamos un objeto de PID y lo configuramos con los parametros ya definidos. El ultimo "DIRECT" indica que el actuador debe actuar directamente sobre la salida para corregir el error. Si fuera "REVERSE", significaria qeu el sistema tendria un comportamiento inverso.
5. Iniciamos las variables de control para  el flujo y posicion del motor.
    - stepperPosition: almacena la posicion actual del motor paso a paso en terminos de pasos. Esta en 0, lo que sugiere que el motor esta en su posicion de referencia.
    - targetFlowRate: almacena la tasa del flujo objetivo que se desea alcanzar. Representa cuantos litros por minuto de gas se desea inyectar.
    - currentFlowRate: almacena la tasa de flujo de gas actual medida con el sensor. Se compara con targetFlowRate para determinar si se necesita ajustar el sistema.
    - valveState: almacena el estado actual de una valvula. Low significa que esta cerrado.
6. void setup()
    Es la funcion qeu se ejecuta cuando se inicia el microcontrolador.
    - Inicializamos los pines: 
        Todos se configuran como salidas. Significa que estos pines se usaran para enviar señales a las valvulas o al generador de O3, para abrir o cerrarlas.
    - Inicializamos el Motor Paso a Paso:
        Establece la velocidad del motor paso a paso en revoluciones por minuto, en nuestro caso en 15rpm.
    - Configuramos el PID:
        - SetMode(AUTOMATIC): Modo automatico. Esto significa que el PID comenzara a calcular automaticamente las salidas en funciones de input y setpoint que se le proporcionen mas adelante en el codigo.
        - SetOutputLimits(-100,100): Limita el rando de salida del PID entre -100 y 100. Es importante porque la salida se utilizara para determianr cuantos pasos debe mover el motor. Limitarlo ayuda a evitar movimientos excesivos o erraticos del motor.
    - Recuperar la posicion del motor:
    Una funcion que simula la recuperacion de la posicion anterior del motor desde una memoria no volatil.
7. void loop()
    - Con la funcion continuousO2Injection() se gestiona la inyeccion continua de oxigeno. Mas adelante se explica la funcion mejor.  
    Analogamente con el resto.
    - Con la funcion continuousMixedGasInjection() se inyecta una mezcla de gases  en las proporciones especificadas. Lo mismo, mas adelante se explica mejor la funcion.
    - La tasa de flujo de O2 (10SLM), garantiza que haya suficiente oxigeno para crear O3 a la cantidad requerida para la aplicacion.
    - La cantidad  de O3 inyectado (7SLM), esta vinculada a la demanda de ozono del sistema, y las proporciones se ajustan en consecuencia para equilibrar el uso de oxigeno y a produccion de ozono.
    - La cantidad de N2 inyectado (5atm), si la presion del tanque supera 5atm , se debe abrir la valvula de liberacion hasta que la presion baje a un nivel seguro. Esto sugiere que 5atm es el limite maximo que el sistema puede manejar sin riesgos de sobrepresion.
    - Como las especificaciones dicen que la presion tiene que estar entre 2atm y 5atm, las tasas se establecen de manera que no sobrepasen esos limites de presion. Si la presion cae por debajo ed 2atm, se activa la bandera de generacion de gas. 
    - En el caso de la inyeccion de gases mixtos los valores como 3SLM de O2, 4SLM de N2 y 2SLM de aire han sido calculados para mantener una mezcla equilibrada, esta diseñado para que el gas inyectado sea homogeneo y seguro. Ademas, las tasas han sido ajustados para optimizar el rendimiento del PID, asegurando qeu las valvulas y el motor respondan de manera eficiente.
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
    - Simular el comportamiento de un sensor real basado en objetos, se puede implementar una logica que intente aproximar el flujo simulado.
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
    - Simulacion completa con variabilidad realista, ideas para simular un comportamiento mas completo, incluyendo fluctuaciones, ajustes dinamicos y limites de flujo.
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
    Simula el almacenamiento del motor paso a paso, que normalmente estaria en la memoria no volatil. En este paso tenemos que añadir algunas cosas:
    - Incluir librerias necesarias, para usar la memoria no volatil(NVS):
        ```cpp
        #include <nvs.h>
        #include <nvs_flash.h>
        ```
    - Incicializar NVS en el setup():
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
    - Implementar la funcion en el loop():
        ```cpp
        void loop() {
            // Código del sistema...
            // Mover motor según sea necesario
            // Actualizar la posición y guardarla en NVS
            storeStepperPosition();
        }
        ```
    - Implementamos la funcion storeStepperPosition(), guardara la posicion del motor en la NVS:
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
    Se necesita tambien una funcion para leer la ultima posicion almacenada cunado el sistema se reinicie:
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
    Mueve el motor paso a paso a una posicion especifica. Calcula cuantos pasos se necesitan y luego actualiza la posicion.
    - Primero controlamos los limites del motor, nos aseguramos que la posicion no baje de 0 y no supere los 2048 pasos.
    - Usamos el PID para regular el flujo de gases. El PID ajusta la posicion del motor en funcion de la tasa de flujo medida y el objetivo(input+setpoint).
12. moveStepper()
    Controla el motor paso a paso. Mueve el motor la cantidad de pasos especificada
13. controlFlow()
    La logica para controlar el flujo del PID. 
    - Ajustamos la velocidad en baso a la desviacion entre setpoint y currentFlowRate.
    - 
14. smoothFlowRate()
    Los sensores de flujo pueden ser ruidosos. Si la tasa de flujo fluctua mucho debido al ruido del sensor, por ello se implementa un filtro de suavizado en las lecturas del sensor para obtener una medida mas estable.
    - Se guardan las ultimas lecturas. El filtro mantiene un historial de lecturas para calcular el promedio. Cuanto mas alto es el numero mas lento se vuelve el filtro.
    - Se agrega la nueva lectura. Llega la nuevo alectura y esta reemplaza a una de las lecturas anteriores.
    - Calcula el promedio. 
    - Devuelve el valor suavizado. Garantiza que los pequeños cambios en la lecutra no afecten la salida del sistema de control.
15. activateValve() y deactivateValve()
    Son las funciones para activar y desactivar las valvulas de acuerdo con el input.
    - Conectadas a los pines del microcontrolador ESP32. Las valvulas estan controladas por MOSFETs, estos activan y desactivan mediante una señal de control un pin digital.
    - Cada valvula tiene asignado un pin especifico.
    - HIGH para abrir y LOW para cerrar.
16. injectGas()
    Esta funcion la verdad es para hacer mas escalable el codigo, encapsula todo el proceso de inyeccion de gas en una unica funcion. Utiliza las funciones ya mencionadas para inyectar el gas.
17. calculateStepsForFlowRate()
    Diseñada para convertir un flujo de gas deseado en la cantidad e pasos que debe moverse el motor paso a paso para alcanzar ese flujo.
    - Define los pasos por revolucion.
    - Define el flujo maximo.
    - Se calculan los pasos por SLM.
18. continuousInjection()
    Son las funciones para los escenarios de inyeccion de gas.
    - O2 inyecta oxigeno aun flujo especifico, maneja la apertura de la valvula, control del flujo y el cierro de la valvula.
    - N2 analogo al de O2.
    - O3 esta funcion es un poco mas compleja, requiere activar el generador de ozono antes de inyectar oxigeno. Al abrir el generador de ozono, se asegura que cuando se inyecta oxigeno, se produce ozono, dado que el generador convierte O2 en O3.
19. continuousMixedGasInjection()
    Diseñada para inyectar una mezcla de gases (O2, N2 y aire filtrado).
    - Los parametros de entrada son las tasas de flujo objetivo que se inyectara.
    - Las valvulas se abren de manera secuencial sin bloquear el sistema, se busca mantener la funcionalidad y la reactividad del sistema mientras se manejan los flujos de gas.
    - La funcion llama tres veces a la funcion para inyectar cada gas:O2, N2 y aire filtrado.

### default_16MB.csv
    DEscribe como se va a particionar la memoria flash de tu ESP32-S3.
    ```default_16MB.csv
    # Name,   Type, SubType, Offset,  Size, Flags
    nvs,      data, nvs,     0x9000,  0x5000,
    otadata,  data, ota,     0xe000,  0x2000,
    app0,     app,  ota_0,   0x10000, 0x310000,
    app1,     app,  ota_1,   0x320000, 0x310000,
    spiffs,   data, spiffs,  0x630000,0x1d0000,
    ```


### Apuntes
(1) Esto define la precision del control, un valor de pasos mas alto significa un movimiento mas fino.  
(2) Un controlador PID es un algoritmo de control utilizado para mantener una variable (en nuestro caso presion) lo mas cerca posible de un valor deseado (setpoint) mediante ajustes automaticos.