from peewee import PostgresqlDatabase
db_config = {
    'host': 'localhost', 
    'port': 5433, 
    'user': 'postgres', 
    'password': 'postgres', 
    'database': 'iot_db'
}

db = PostgresqlDatabase(**db_config)

a=db.execute_sql("SELECT * FROM Config WHERE id = 1").fetchone()
print(a)