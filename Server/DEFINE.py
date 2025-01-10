DATABASE_MEASUREMENT = 'Server/database/measurement.db'
DATABASE_DEVICES = 'Server/database/devices.db'
DATABASE_LOG = 'Server/database/log.db'

       
SCRIPT_INSERT_MEASUREMENT = "INSERT INTO measurement (id, temperature, time_of_measurement) VALUES (?, ?, ?)"
SCRIPT_INSERT_DEVICES = "INSERT INTO devices (ip, id, descirption, time_of_connection) VALUES (?, ?, ?, ?)"

INIT_SCRIPT_DEVICES = 'Server/sql/device_init.sql'
INIT_SCRIPT_MEASUREMENT = 'Server/sql/measurement_init.sql'

SCRIPT_READ_ID = "SELECT ip, id FROM devices"
SCRIPT_SEARCH_ID = "SELECT id FROM devices WHERE ip = ?"
SCRIPT_MAX_ID = "SELECT MAX(id) FROM devices"

SCRIPT_READ_LAST_MEASUREMENT = f"SELECT * FROM measurement ORDER BY time_of_measurement DESC LIMIT 1;"
SCRIPT_READ_LAST_LOG = f"SELECT * FROM logs ORDER BY timestamp DESC LIMIT 1;"