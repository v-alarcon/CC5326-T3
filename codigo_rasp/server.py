from modelos import *
import socket
import datetime
import struct
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
                    if config.ID_protocol == "3" and config.Transport_Layer == "0":
                        print("\nPrepare to receive data from Protocol 3")
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

                        #get the timestamp
                        timestamp0 = int.from_bytes(data[13:17], signed=False, byteorder='little')
                        ts = datetime.datetime.fromtimestamp(timestamp0).strftime('%Y-%m-%d %H:%M:%S')
                        print("Timestamp: " + ts)

                        #get the temperature from the next byte
                        temperature = data[17]
                        print("Temperature: " + str(temperature))

                        #get the pressure from the next 4 bytes
                        pressure = int.from_bytes(data[18:22], signed=False, byteorder='little')
                        print("Pressure: " + str(pressure))

                        #get the humidity from the next byte its an int
                        humidity = data[22]
                        print("Humidity: " + str(humidity))

                        #get the CO2 from the next 4 bytes, its a float
                        co = struct.unpack('f', data[23:27])
                        print("CO: " + str(co[0]))

                        #get the RMS from the next 4 bytes, its a float
                        rms = struct.unpack('f', data[27:31])
                        print("RMS: " + str(rms[0]))

                        #get the ampx from the next 4 bytes, its a float
                        ampx = struct.unpack('f', data[31:35])
                        print("AMPX: " + str(ampx[0]))

                        #get the frecx from the next 4 bytes, its a float
                        frecx = struct.unpack('f', data[35:39])
                        print("FRECX: " + str(frecx[0]))

                        #get the ampy from the next 4 bytes, its a float
                        ampy = struct.unpack('f', data[39:43])
                        print("AMPY: " + str(ampy[0]))

                        #get the frecy from the next 4 bytes, its a float
                        frecy = struct.unpack('f', data[43:47])
                        print("FRECY: " + str(frecy[0]))

                        #get the ampz from the next 4 bytes, its a float
                        ampz = struct.unpack('f', data[47:51])
                        print("AMPZ: " + str(ampz[0]))

                        #get the frecz from the next 4 bytes, its a float
                        frecz = struct.unpack('f', data[51:55])
                        print("FRECZ: " + str(frecz[0]))

                        #save the data in the database
                        Datos.insert(Id_device=id_device, MAC=mac, battlevel=battery_level, timestamp=ts, temp=temperature,
                                     press=pressure, hum=humidity, co=co[0], rms=rms[0], ampx=ampx[0], freqx=frecx[0],
                                     ampy=ampy[0], freqy=frecy[0], ampz=ampz[0], freqz=frecz[0]).execute()
                        Logs.insert(ID_device=id_device, Transport_Layer=transport_layer, finaltime=datetime.datetime.now(),
                                        initialtime=ts).execute()



            