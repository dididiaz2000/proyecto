##### PROYECTO #####
# Comienzo
- He empezado con el proyecto y compila bien, lo que es la plataforma como tal, este commit es el bueno por ahora. 

<!------ platformio.ini ------------->
    - Para que me funcionen todas las bibliotecas que necesitaba he añadido una linea de codigo mas en la configuración:

    $ lib_deps = Stepper

    - Tambien he tenido que cambiar una linea:

    $ upload_flash_size = 16

    por: $ board_flash_size = 16

    - Por facilidad he aliminado el segundo framework de "espidf".
    - En el sdkconfig era encesario cambiar el "CONFIG_FREERTOS_HZ" de 100 a 1000.

Corrigiendo estos errores ya funciona el 'platformio.ini'.

<!------ main.cpp ------------->
    - He añadido los includes necesarios.
    - El codigo tiene muchas funciones que en principio deberian funcionar.
    - El primer intento de compilacion lo deberia hacer el miercoles si todo va bien.
