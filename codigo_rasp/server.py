from modelos import *
import socket
import datetime

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
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        while True:
            input("Press Enter to continue...")
            # ask to the table Config of database db using peewee by id
            # ask to the table Config of database db using peewee by id
            config = Config.get_by_id(1)
            # if fisical layer = 1 it means we need to use wifi, else we use BLE
            print("Fisical layer: " + str(config.Fisical_Layer))
            # id of the protocol to generate the data
            print("Protocol: " + str(config.ID_protocol))
            # transport layer to use (0 for ycp and 1 for tcp, in BLE 0 is for continue and 1 for sleep)
            print("Transport layer: " + str(config.Transport_Layer))
            print("El servidor está esperando conexiones en el puerto", PORT)

            conn, addr = s.accept()
            with conn:
                print('Conectado por', addr)
                conn.sendall((str(config.Fisical_Layer)+str(config.ID_protocol)+str(config.Transport_Layer)).encode('utf-8'))
                # if fisical layer is 1 it means we need to use wifi, else we use BLE
                if config.Fisical_Layer == "1":
                     if config.ID_protocol == "0" and config.Transport_Layer == "0":
                        # use the protocol 0 using tcp
                        print("\nPrepare to receive data from Protocol 0")
                        s.listen()
                        conn, addr = s.accept()
                        print('Conectado por', addr)
                        data = conn.recv(1024)
                        print("Recibido los bytes")
                        # save data in the database
                        print("Guardando en la base de datos")
                        
                        #get the first 2 bytes from the data and transform into a str id_device
                        id_device = str(data[0:2].decode('utf-8'))
                        print("ID: " + id_device)

                        #get the next 6 bytes from the data and transform into a str MAC
                        mac = ""
                        for i in range(4):
                            mac += bytes.hex(data[2+i:3+i]) + ":"
                        mac = mac[:-1]
                        print("MAC: " + mac)

                        #get the next byte to know the transport layer
                        transport_layer = chr(data[8])
                        print("Transport layer: " + transport_layer)

                        #get the next byte to get the protocol
                        protocol = chr(data[9])
                        print("Protocol: " + protocol)

                        #get the next 2 bytes to get the packet length
                        packet_length = int.from_bytes(data[10:12], signed=False, byteorder='little')
                        print("Packet length: " + str(packet_length))

                        #get the final byte with the battery level
                        battery_level = data[12]
                        print("Battery level: " + str(battery_level))

                        #save the data in the database
                        #Config.insert(ID_protocol='0', Transport_Layer='0').execute()
                        Datos.insert(Id_device=id_device, MAC=mac, battlevel=battery_level).execute()
                        Logs.insert(ID_device=id_device, Transport_Layer=transport_layer, finaltime=datetime.datetime.now()).execute()

            