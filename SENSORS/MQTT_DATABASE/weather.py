import sqlalchemy as db
from sqlalchemy.dialects.mysql import insert
import paho.mqtt.client as mqtt
import json
import traceback
import time

engine = db.create_engine("mysql+pymysql://user:123456@localhost/machine?charset=latin1&binary_prefix=true",
                          encoding='latin1',
                          pool_pre_ping=True,
                          pool_size=5,
                          pool_recycle=1200,
                          max_overflow=10,
                          pool_timeout=20)
connection = engine.connect()
metadata = db.MetaData()
mc = db.Table('weather', metadata, autoload=True, autoload_with=engine)
connection.close()

def insert_record(msg_dict):
    try:
        connection = engine.connect()
        
        try:
            connection.execute(mc.insert(),msg_dict) #{"machine_name":"ok","quantity":"1"}
        except ValueError as e:
            traceback.print_exc()
            
        connection.close()
        
    except ValueError as e:
        traceback.print_exc()
        
current_time = time.time()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("MQTT Connected")

def on_message(client, userdata, message): 
    global current_time
    msg = str(message.payload.decode("utf-8"))
    try:               
        json_object = json.loads(msg)      
        #json_object = {"temp":msg}
        print(json_object)
        if (time.time()-current_time>30): 
            insert_record(json_object)
            current_time = time.time()
            
    except ValueError as e:
        traceback.print_exc()
     
    print("message received " ,msg)
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)  
    
def mqtt_start():
    global mqtt_client
    mqtt_client = mqtt.Client("server")
    mqtt_client.on_connect=on_connect
    mqtt_client.username_pw_set(username="IOT14",password="P14")
    mqtt_client.connect("localhost", 1883,60)    
    mqtt_client.loop_start()
    
global mqtt_client
mqtt_start()
mqtt_client.subscribe("Q/W")
mqtt_client.on_message = on_message

while True:
	time.sleep(1)
