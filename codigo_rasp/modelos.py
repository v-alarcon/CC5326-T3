from peewee import *
from playhouse.postgres_ext import ArrayField
# Configuración de la base de datos
db_config = {
    'host': 'localhost', 
    'port': 5433, 
    'user': 'postgres', 
    'password': 'postgres', 
    'database': 'iot_db'
}
db = PostgresqlDatabase(**db_config)

# Definición de un modelo
class BaseModel(Model):
    class Meta:
        database = db

# create a new table named Datos with timestamp, Id_device and MAC
class Datos(BaseModel):
    Id_device = CharField()
    MAC = CharField()
    battlevel = IntegerField()
    timestamp = DateTimeField(null = True)

    temp = IntegerField(null = True)
    press = IntegerField(null = True)
    hum = IntegerField(null = True)    
    co = FloatField(null = True)

    ampx = FloatField(null = True)
    freqx = FloatField(null = True)
    ampy = FloatField(null = True)
    freqy = FloatField(null = True)
    ampz = FloatField(null = True)
    freqz = FloatField(null = True)
    rms = FloatField(null = True)


    accx = ArrayField(FloatField, null = True)
    accy = ArrayField(FloatField, null = True)
    accz = ArrayField(FloatField, null = True)
    rgyrx = ArrayField(FloatField, null = True)
    rgyry = ArrayField(FloatField, null = True)
    rgyrz = ArrayField(FloatField, null = True)
    

# create a new table named Logs with ID_device, Transport_Layer and timestamp
class Logs(BaseModel):
    ID_device = CharField()
    Transport_Layer = CharField()
    initialtime = DateTimeField(null = True)
    finaltime = DateTimeField()

# create a new table named Config with Fisical layer, ID_protocol and Transport_Layer
class Config(BaseModel):
    fisical = CharField()
    idprotocol = CharField()
    transportlayer = CharField()

# create a new table named Loss with timestap and packet_loss
class Loss(BaseModel):
    timestamp = DateTimeField()
    packet_loss = CharField()

# create the tables in the database
db.create_tables([Datos, Logs, Config, Loss])
