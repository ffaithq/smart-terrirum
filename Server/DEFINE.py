DATABASE_MEASUREMENT = 'Server/database/measurement.db'
DATABASE_DEVICES = 'Server/database/devices.db'
DATABASE_LOG = 'Server/database/log.db'
DATABASE_SETPOINTS = 'Server/database/setpoints.db'
DATABASE_USERS = 'Server/database/users.db'

       
SCRIPT_INSERT_MEASUREMENT = "INSERT INTO measurement (id, temperature, time_of_measurement) VALUES (?, ?, ?)"
SCRIPT_INSERT_DEVICES = "INSERT INTO devices (ip, id, descirption, time_of_connection) VALUES (?, ?, ?, ?)"
SCRIPT_INSERT_SETPOINS = 'INSERT INTO setpoints (name,waterlevel,uvlevel,image_filename) VALUES (?, ?, ?, ?)'
SCRIPT_INSERT_USER = "INSERT INTO users (username, password_hash, usertype) VALUES (?, ?,?)"

INIT_SCRIPT_DEVICES = 'Server/sql/device_init.sql'
INIT_SCRIPT_MEASUREMENT = 'Server/sql/measurement_init.sql'
INIT_SCRIPT_SETPOINTS = 'Server/sql/setpoints_db.sql'
INIT_SCRIPT_USERS = 'Server/sql/users.sql'


SCRIPT_READ_ID = "SELECT ip, id FROM devices"
SCRIPT_SEARCH_ID = "SELECT id FROM devices WHERE ip = ?"
SCRIPT_MAX_ID = "SELECT MAX(id) FROM devices"
SCRIPT_READ_SETPOINTS_DATABASE = "SELECT * FROM setpoints"
SCRIPT_READ_IMAGE_SETPOINTS = "SELECT image_filename FROM setpoints WHERE id = ?"
SCRITP_SEARCH_FOR_USER = "SELECT id, password_hash, usertype FROM users WHERE username = ?"
SCRITP_SEARCH_FOR_USER = "SELECT id, password_hash,usertype FROM users WHERE username = ?"
SCRIPT_READ_LAST_MEASUREMENT = "SELECT * FROM measurement ORDER BY time_of_measurement DESC LIMIT 1;"
SCRIPT_READ_LAST_LOG = "SELECT * FROM logs ORDER BY timestamp DESC LIMIT 1;"
SCRIPT_DELETE_SETPOINT = "DELETE FROM setpoints WHERE id = ?"

SCRIPT_DEACTIVATE_ALL = "UPDATE setpoints SET active = 0"
SCRIPT_ACTIVATE_ONE = "UPDATE setpoints SET active = 1 WHERE id = ?" 

SCRIPT_UPDATE_NAME = "UPDATE setpoints SET name = ? WHERE id = ?" 
SCRIPT_UPDATE_WATERLEVEL = "UPDATE setpoints SET waterlevel = ? WHERE id = ?" 
SCRIPT_UPDATE_UVLEVEL = "UPDATE setpoints SET uvlevel = ? WHERE id = ?" 

UPLOAD_PATH = "Server/img"
REAL_PATH = "img"


ALLOWED_EXTENSION = ('png', 'jpg', 'jpeg', 'gif')

ADMIN_USERNAME = 'vsb-tuo'
ADMIN_PASSWORD = 'kat450'