# PROYECTO 
## Comienzo
He empezado con el proyecto y compila bien, lo que es la plataforma como tal, este commit es el bueno por ahora. 

### platformio.ini
Para que me funcionen todas las bibliotecas que necesitaba he añadido una linea de codigo mas en la configuración:

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
    

### Apuntes
(1) Esto define la precision del control, un valor de pasos mas alto significa un movimiento mas fino.  
(2) Un controlador PID es un algoritmo de control utilizado para mantener una variable (en nuestro caso presion) lo mas cerca posible de un valor deseado (setpoint) mediante ajustes automaticos.