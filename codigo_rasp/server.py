from modelos import *

# Datos WIFI
HOST = '0.0.0.0'  # Escucha en todas las interfaces disponibles
PORT = 1234       # Puerto en el que se escucha

# Configuración de la base de datos
db_config = {
    'host': 'localhost', 
    'port': 5432, 
    'user': 'postgres', 
    'password': 'postgres', 
    'database': 'db'
}

db = PostgresqlDatabase(**db_config)

while True:
    input("Press Enter to continue...")
    # ask to the table Config of database db using peewee by id
    config = Config.get_by_id(1)
    print(config.Fisical_Layer)
    print(config.ID_protocol)
    print(config.Transport_Layer)
    