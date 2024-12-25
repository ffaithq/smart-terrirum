import paho.mqtt.client as mqtt
import time
import sqlite3
import os
import json


# init database where will be stored ip of esp and that id they got
def init_db(file_path,script_path):
    # 0 - means was create successufly
    # 1 - means database is already exist - NOT ERROR
    # 2 - error while creating database file - ERROR
    # 3 - error while connecting - ERROR
    # 4 - error while creating table - ERROR
    if os.path.exists(file_path):
        with open(script_path, 'r') as f:
            sql_script = f.read()

        conn = sqlite3.connect(file_path)
        cursor = conn.cursor()
        # 3. Execute the entire script
        cursor.executescript(sql_script)
        # Commit changes to the database
        conn.commit()
        return 1

    try:
        # Connect to SQLite database (or create it if it doesn't exist)
        conn = sqlite3.connect(file_path)
    except:
        return 2

    try:    
        # Create a cursor object to execute SQL queries
        cursor = conn.cursor()
    except:
        return 3

    try:
        with open(script_path, 'r') as f:
            sql_script = f.read()

        # 3. Execute the entire script
        cursor.executescript(sql_script)
        # Commit changes to the database
        conn.commit()
    except:
        return 4
    
    return 0


def get_connection(file_path):
    if not os.path.exists(file_path):
        return None
    conn = sqlite3.connect(file_path)
    conn.row_factory = sqlite3.Row  # Allows rows to be accessed as dictionaries
    return conn

def on_publish(client, userdata, mid):
    """
    Called when a message that was to be sent using the publish() call 
    has completed transmission to the broker.
    """
    print(f"[on_publish] Message ID: {mid}")

# a callback functions 
def callback_list_ids(client, userdata, msg):
    print('List of ids: ', msg.payload.decode('utf-8'))

def get_ids_list():
    """
    Retrieve all rows of `ip` and `id` from the specified table,
    and return them as a JSON string.
    """
    # Connect to the database
    conn = get_connection('SW/database/devices.db')
    cursor = conn.cursor()

    # Execute a query to select IP and ID
    cursor.execute(f"SELECT ip, id FROM devices")
    # Fetch all rows
    rows = cursor.fetchall()
    # rows will look like: [("192.168.0.10", 1), ("192.168.0.11", 2), ...]

    # Convert rows to a list of dictionaries
    results = {}
    for row in rows:
        ip, device_id = row
        results[ip] = device_id

    # Close the database connection
    cursor.close()
    conn.close()

    # Convert to JSON
    json_data = json.dumps(results, indent=4)
    return json_data

def publish_ids_list(client, qos=0, retain=False):
    msg = get_ids_list()
    pubMsg = client.publish(
        topic='rpi/list_ids',
        payload=msg.encode('utf-8'),
        qos=0,
    )
    pubMsg.wait_for_publish()

def check_ip_in_db(ip_address):
    # Connect to the SQLite database
    conn = sqlite3.connect('SW/database/devices.db')
    cursor = conn.cursor()
    
    # Query to check if the IP exists
    query = "SELECT id FROM devices WHERE ip = ?"
    cursor.execute(query, (ip_address,))
    result = cursor.fetchone()
    
    # Close the connection
    conn.close()
    
    # Return the ID if the IP exists, otherwise return None
    if result:
        return result[0]
    else:
        return None

def client_subscriptions(client):
    # TODO: add more subscriptions
    client.subscribe("rpi/conn_esp")
    client.subscribe("rpi/list_ids")

def on_connect(client, userdata, flags, rc):
   global flag_connected
   flag_connected = 1
   client_subscriptions(client)
   print("Connected to MQTT server")

def on_disconnect(client, userdata, rc):
   global flag_connected
   flag_connected = 0
   print("Disconnected from MQTT server")

def lookup_id(ip):
    conn = get_connection('SW/database/measurement.db')
    cursor = conn.cursor()
    cursor.execute("""
        SELECT id 
        FROM devices 
        WHERE ip = ?
    """, (ip,))
    id = cursor.fetchone()[0]
    if id is None:
        cursor.close()
        conn.close()
        return None
    
    cursor.close()
    conn.close()
    return id

def callback_esp32(client, userdata, msg):
    try:
        conn = get_connection('SW/database/measurement.db')
    except ValueError as e:
        print("Caught an error while getting connection to database 'database/measurement.db', error:", e)

    try:
        cursor = conn.cursor()
    except ValueError as e:
        print("Caught an error while getting cursor to database 'database/measurement.db', error:", e)
        conn.close()
        return
    
    try:
        # TODO: add more fields to the table
        payload_str = msg.payload.decode('utf-8')
        msg_json = json.loads(payload_str)
        id = msg_json['deviceId']
        if id is None:
            print("ID is None")
            return
        #id = lookup_id(ip)
        temperature = msg_json['temperature']
        time_of_measurement = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        cursor.execute("INSERT INTO measurement (id, temperature, time_of_measurement) VALUES (?, ?, ?)", (id,temperature,time_of_measurement))
        conn.commit()
        cursor.close()
        conn.close()
        print(f"Inserted data into database for ID {id} with temperature {temperature} and time {time_of_measurement}")
    except json.JSONDecodeError:
        print("Failed to decode JSON from payload:", msg.payload)

def callback_conn_esp(c, userdata, msg):
    try:
        conn = get_connection('SW/database/devices.db')
    except ValueError as e:
        print("Caught an error while getting connection to database 'database/devices.db', error:", e)

    try:
        cursor = conn.cursor()
    except ValueError as e:
        print("Caught an error while getting cursor to database 'database/devices.db', error:", e)
        conn.close()
        return


    
    try:
        payload_str = msg.payload.decode('utf-8')
        msg_json = json.loads(payload_str)
        ip = msg_json['IP']
        desription = msg_json['Description']
        try:
            last_id = check_ip_in_db(ip)
            if last_id is None:
                cursor.execute("SELECT MAX(id) FROM devices")
                # 4. Fetch the result
                last_id = cursor.fetchone()  # returns a tuple, e.g., (42,) if 42 is the max id
                if last_id[0] is None:
                    last_id = 0
                    id = last_id + 1
                    time_of_connection = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
                    cursor.execute("INSERT INTO devices (ip, id, descirption, time_of_connection) VALUES (?, ?, ?, ?)", (ip,id,desription,time_of_connection))
                    conn.commit()
                    cursor.close()
                    conn.close()
                    client.subscribe(f"esp/id_{id}")
                    print(f"esp/id_{id}")
                    client.message_callback_add(f"esp/id_{id}", callback_esp32)
                    print(f"Inserted data into database for IP {ip} with id {id} and time {time_of_connection}")
            else:
                print(f"Record with IP {ip} already exists in the database")
                client.subscribe(f"esp/id_{last_id}")
                client.message_callback_add(f"esp/id_{last_id}", callback_esp32)
        except ValueError as e:
            print("Caught an error while getting last id from database 'database/devices.db', error:", e)
            conn.close()
            return

    except json.JSONDecodeError:
        print("Failed to decode JSON from payload:", msg.payload)


init_db('SW/database/devices.db','SW/sql/device_init.sql')
init_db('SW/database/measurement.db','SW/sql/measurement_init.sql')
client = mqtt.Client("rpi") #this should be a unique name
flag_connected = 0
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish
client.message_callback_add('rpi/conn_esp', callback_conn_esp)
client.message_callback_add('rpi/list_ids', callback_list_ids)
client.connect('192.168.0.175',1883)

client.loop_start()
client_subscriptions(client)
print("......client setup complete............")

while True:
    time.sleep(1)
    publish_ids_list(client)
    if (flag_connected != 1):
        print("trying to connect MQTT server..")