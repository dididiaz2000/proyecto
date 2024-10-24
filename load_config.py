import json
import os
import sys

def load_config():
    with open('config.json') as f:
        config = json.load(f)

    # Aquí puedes manipular la configuración como desees
    # Por ejemplo, puedes establecer variables de entorno
    os.environ['FLASH_SIZE'] = config['upload']['flash_size']
    # Agrega más variables según sea necesario

if __name__ == "__main__":
    load_config()
