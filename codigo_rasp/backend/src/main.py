from peewee import *
from fastapi import FastAPI, Depends
from pydantic import BaseModel
from starlette.middleware.cors import CORSMiddleware

# -------------------------
# Conexion a la base de datos
# -------------------------

db = PostgresqlDatabase(
    'iot_db',
    user='postgres',
    password='postgres',
    host='192.168.4.1',  # Si usan docker compose, el host es el nombre del servicio, si no, es localhost
    port='5433'
)
def get_database():
    db.connect()
    try:
        yield db
    finally:
        if not db.is_closed():
            db.close()

# -------------------------
# Creacion de la app
# -------------------------


app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# -------------------------
# Aqui pueden colocar sus endpoints
# -------------------------


class SetSetting(BaseModel):
    # Esto de ocupa para definir que recive un endpoint
    fisical_layer: str
    id_protocol: str
    transport_layer: str


@app.get("/conf/")
async def get_items(database: PostgresqlDatabase = Depends(get_database)):
    # Aqui pueden acceder a la base de datos y hacer las consultas que necesiten
    # ask to the db for the table Config using peewee by id without using the model
    a=db.execute_sql("SELECT * FROM Config WHERE id = 1").fetchone()
    print(a)

    return {"values" : a}


@app.post("/conf/")
async def create_item(setting: SetSetting, database: PostgresqlDatabase = Depends(get_database)):
    # aqui reciben un objeto de tipo Setting
    setting_dict = setting.dict()
    print(setting_dict)
    # Luego pueden usar la base de datos
    #update the config table with the new values
    db.execute_sql("UPDATE Config SET fisical = %s, idprotocol = %s, transportlayer = %s WHERE id = 1", (setting_dict["fisical_layer"], setting_dict["id_protocol"], setting_dict["transport_layer"]))
    return {"message": "db updated"}
