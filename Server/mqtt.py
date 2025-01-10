import paho.mqtt.client as mqtt
import time
import json
from database import Database
import logging
from logger import DatabaseLogHandler
from DEFINE import *


#logger.warning("Warning message")
#logger.error("Error message")
# TODO: SETUP JSON FILE
# TODO: Documentation
# TODO: Improve logging


class MQTTClientHandler:
    def __init__(self):

        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2) #this should be a unique name
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_publish = self.on_publish
        self.client.message_callback_add('rpi/conn_esp', self.callback_conn_esp)
        self.client.message_callback_add('rpi/list_ids', self.callback_list_ids)
        self.client.connect('192.168.0.175',1883)
        self.logger = self.setup_logger(DATABASE_LOG)
        self.devices = Database(path=DATABASE_DEVICES,
                          init_script=INIT_SCRIPT_DEVICES)
        self.measurement = Database(path=DATABASE_MEASUREMENT,
                          init_script=INIT_SCRIPT_MEASUREMENT)
        self.flag_connected = 0

    def setup_logger(self,db_file):

        logger = logging.getLogger("DatabaseLogger")
        logger.setLevel(logging.DEBUG)

        db_handler = DatabaseLogHandler(db_file)
        formatter = logging.Formatter('%(levelname)s - %(message)s')
        db_handler.setFormatter(formatter)

        logger.addHandler(db_handler)
        return logger

    def on_connect(self,client, userdata, flags, reason_code, properties):
        if reason_code == 0:
            self.logger.info(f"[on connection] Connected successfully!")
            self.flag_connected = 1
            self.client_subscriptions()
        else:
            self.logger.error(f"[on connection] Connection failed with reason code {reason_code}")


    def on_disconnect(self,client, userdata, flags, reason_code, properties):
        self.flag_connected = 0
        self.logger.info(f"Disconnected from MQTT server")

    def on_publish(self,client, userdata, mid, reason_code, properties):
        """
        Called when a message that was to be sent using the publish() call 
        has completed transmission to the broker.
        """

        self.logger.info(f"[on_publish] Message ID: {mid}")

    # a callback functions 
    def callback_list_ids(self,client, userdata, msg):
        self.logger.info(f'List of ids: {msg.payload.decode("utf-8")}')

    def __decode_id(self,rows) -> json:
        results = {}
        if rows is not None:
            for row in rows:
                ip, device_id = row
                results[ip] = device_id


        json_data = json.dumps(results, indent=4)
        return json_data

    def __asign_id(self,ip):
        id = self.devices.read_data(SCRIPT_SEARCH_ID,data=ip)
        status = 0
        if id is None:
            status = 1
            id = self.devices.read_data(SCRIPT_MAX_ID)[0]
            if id is None:
                id = 0
                id += 1

        else:
            id = id[0]
        return id,status

    def publish_ids_list(self,client, qos=0, retain=False):
        rows = self.devices.read_data(query=SCRIPT_READ_ID,fetch='all')

        msg = self.__decode_id(rows=rows)

        pubMsg = client.publish(
            topic='rpi/list_ids',
            payload=msg.encode('utf-8'),
            qos=0,
        )
        pubMsg.wait_for_publish()

    def client_subscriptions(self):
        # TODO: add more subscriptions
        self.client.subscribe("rpi/conn_esp")
        self.client.subscribe("rpi/list_ids")

    def callback_esp32(self,client, userdata, msg):    
        try:
            payload_str = msg.payload.decode('utf-8')
            msg_json = json.loads(payload_str)
            id = msg_json['deviceId']
            if id is None:
                self.logger.info("ID is None")
                return
            
            temperature = msg_json['temperature']
            time_of_measurement = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())

            self.measurement.write_data(script=SCRIPT_INSERT_MEASUREMENT,
                                        data=(id,temperature,time_of_measurement))
            
            self.logger.info(f"Inserted data into database for ID {id} with temperature {temperature} and time {time_of_measurement}")
        except json.JSONDecodeError:
            self.logger.info(f"Failed to decode JSON from payload:{payload_str}")

    def callback_conn_esp(self,c, userdata, msg):
        """
            Callback function. Execute as callback on topic rpi/conn_esp. 
            While start up ESP32 will always send first request on this topic. 
            Handshake between RPI and ESP.
            
            Arguments:
                C - client: passed by paho function
                userdata - userdata which are passed by paho function
                msg - message which was arrived on topic rpi/conn_esp

            Return:
                Nothing
        """
        try:
    
            payload_str = msg.payload.decode('utf-8')
            msg_json = json.loads(payload_str)

            ip = msg_json['IP']
            desription = msg_json['Description']
            time_of_connection = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())

            id,status = self.__asign_id(ip=ip)

            if status == 1:
                self.logger.info(f"esp/id_{id}")
                self.devices.write_data(SCRIPT_INSERT_DEVICES,(ip,id,desription,time_of_connection))
                self.logger.info(f"Inserted data into database for IP {ip} with id {id} and time {time_of_connection}")
            if status == 0:
                self.logger.info(f"Record with IP {ip} already exists in the database")


            self.client.subscribe(f"esp/id_{id}")
            self.client.message_callback_add(f"esp/id_{id}", self.callback_esp32)

        except json.JSONDecodeError:
            self.logger.info(f"Failed to decode JSON from payload: {msg.payload}")

    def run_server(self):
        self.client.loop_start()
        self.client_subscriptions()
        self.logger.info(f"client setup complete")

        while True:
            time.sleep(1)
            self.publish_ids_list(self.client)
            if (self.flag_connected != 1):
                self.logger.info(f"trying to connect MQTT server..")








