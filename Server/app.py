from flask import Flask, render_template, request, jsonify, send_file
import sqlite3
import threading
import time
import snap7

import csv

app = Flask(__name__, static_folder="static", template_folder="templates")

# Global variables to simulate PLC
IsConnected = False
LEDs = []
MODE = None
SPEED = None
DIRECTION = None
VOLTAGE = None
TIME = None


# SQLite Database connection
def get_db_connection():
    conn = sqlite3.connect("measurement.db")
    conn.row_factory = sqlite3.Row  # Allows rows to be accessed as dictionaries
    return conn


# Routes
@app.route("/")
def index():
    return render_template("index.html")


@app.route("/data", methods=["GET"])
def get_data():
    global IsConnected, LEDs, MODE, SPEED, DIRECTION, VOLTAGE,TIME

    if IsConnected:
        return jsonify({
            "connect": IsConnected,
            "led": LEDs,
            "mode": MODE,
            "speed": SPEED,
            "direction": DIRECTION,
            'TIME':TIME,
            "voltage": VOLTAGE
        })
    return jsonify({"connect": IsConnected})

@app.route("/change_mode", methods=["POST"])
def change_mode():
    global IsConnected, plcTo
    if IsConnected:
        data = request.json
        mode_value = data.get("value")
        buffer = bytearray(2)
        snap7.util.set_int(buffer, 0, mode_value)
        plcTo.db_write(3, 0, buffer)
        return jsonify({"status": "success"})
    return jsonify({"status": "failed"}), 400


@app.route("/change_speed", methods=["POST"])
def change_speed():
    global IsConnected,plcTo
    if IsConnected:
        data = request.json
        speed_value = data.get("value")
        buffer = bytearray(2)
        snap7.util.set_int(buffer, 0, speed_value)
        plcTo.db_write(3, 2, buffer)
        # Simulate PLC write logic here
        return jsonify({"status": "success"})
    return jsonify({"status": "failed"}), 400


@app.route("/change_direction", methods=["POST"])
def change_direction():
    global IsConnected, plcTo
    if IsConnected:
        data = request.json
        direction_value = data.get("value")
        buffer = bytearray(1)
        snap7.util.set_bool(buffer, 0,0, direction_value)
        plcTo.db_write(3, 4, buffer)
        return jsonify({"status": "success"})
    return jsonify({"status": "failed"}), 400

@app.route("/charging", methods=["POST"])
def charging():
    global IsConnected, plcTo
    if IsConnected:
        buffer = bytearray(1)
        snap7.util.set_bool(buffer, 0,1, True)
        plcTo.db_write(3, 4, buffer)
        return jsonify({"status": "success"})
    return jsonify({"status": "failed"}), 400

@app.route("/discharging", methods=["POST"])
def discharging():
    global IsConnected, plcTo
    if IsConnected:
        buffer = bytearray(1)
        snap7.util.set_bool(buffer, 0,2, True)
        plcTo.db_write(3, 4, buffer)
        return jsonify({"status": "success"})
    return jsonify({"status": "failed"}), 400

@app.route("/connect", methods=["POST"])
def connect():
    global IsConnected, plcTo,plcFrom
    data = request.json
    ip = data.get("ip")
    rack = data.get("rack")
    slot = data.get("slot")
    try:
        plcTo.connect(ip, int(rack), int(slot))
        plcFrom.connect(ip, int(rack), int(slot))
        IsConnected = True
        return jsonify({"status": "success"})
    except Exception as e:
        IsConnected = False
        return jsonify({"status": f"error: {str(e)}"}), 400


@app.route("/disconnect", methods=["POST"])
def disconnect():
    global IsConnected, plcTo,plcFrom
    IsConnected = False
    plcTo.disconnect()
    plcFrom.disconnect()
    return jsonify({"status": "success"})


@app.route("/download-csv", methods=["GET"])
def download_csv():
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM measurement")
    rows = cursor.fetchall()
    conn.close()

    file_path = "measurement.csv"

    with open(file_path, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["data", "voltage"])
        for row in rows:
            writer.writerow([row["data"], row["voltage"]])

    return send_file(file_path, as_attachment=True, mimetype="text/csv", download_name="measurement.csv")


def run_server():
    app.run(host="0.0.0.0", port=8000, debug=False)


def plc_fetch():
    # shared variables
    global LEDs, MODE, SPEED, VOLTAGE, DIRECTION,TIME, plcFrom, IsConnected
    # connection to database
    conn = get_db_connection()
    # cursor databaset to operate with database
    cursor = conn.cursor()

    # Infinite loop
    while True:
        # Delay between reading
        time.sleep(0.1)
        if IsConnected:
            # Read DB1 from 0 byte to 24 bytes not include
            try:
                data = plcFrom.db_read(1, 0, 24)
            except:
                IsConnected = False
                continue
            # Reset list
            LEDs.clear()
            # Decode data from plc
            for i in range(8):
                LEDs.append(snap7.util.get_bool(data, 0, i))

            MODE = snap7.util.get_int(data, 2)
            SPEED = snap7.util.get_int(data, 4)
            DIRECTION = snap7.util.get_bool(data, 6, 0)
            VOLTAGE = snap7.util.get_real(data, 8)
            TIME = snap7.util.get_dtl(data, 12)

            # Save data (voltage on analog input, time from PLC) from DB1 to database 
            cursor.execute("INSERT INTO measurement (data, voltage) VALUES (?, ?)", (TIME, VOLTAGE))
            conn.commit()


# Initialize the database
def initialize_database():
    conn = get_db_connection()
    cursor = conn.cursor()
    cursor.execute("""
    CREATE TABLE IF NOT EXISTS measurement (
        data DATETIME,
        voltage REAL
    )
    """)
    conn.commit()
    conn.close()


# Run the application
if __name__ == "__main__":
    # Initialize global variables
    plcTo = snap7.client.Client()
    plcFrom = snap7.client.Client()
    initialize_database()

    # Create threads for PLC fetching and Flask server
    plc_thread = threading.Thread(target=plc_fetch, daemon=True)
    server_thread = threading.Thread(target=run_server, daemon=True)

    # Start threads
    plc_thread.start()
    server_thread.start()

    # Keep the main thread alive
    plc_thread.join()
    server_thread.join()
