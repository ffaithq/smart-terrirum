import sqlite3
import os
import json
from DEFINE import *

"""

        Status code:
            0 - means was create successufly.
            1 - error while creating database file - ERROR.
            2 - error while connecting - ERROR.
            3 - error while creating table - ERROR.
    
"""

class Database:
    def __init__(self,path,init_script):
        self.path = path
        self.init_script = init_script
        self.status = -1
        if init_script is not None:
            self.__init_db()

    def __init_db(self):
        """
        function to init database -> int
        Function will connect to database. If database isn't existed before SQLite will handle and create new database.
        After creating connection will execute script. In script should be noticed that in case if table already exists do not create new one.
        Otherwise it can overwrite already table during start up
        Argument: 
            file_path: str - path to database file. Can be any string which representrs path
            script_path: str - path to SQL script. Script has to contain initialisation of table in database.

        Return:
            Status code: int 

        """
        try:
            conn = sqlite3.connect(self.path,check_same_thread=False)
        except:
            self.status = 1
            return 
        
        # Create a cursor object to execute SQL queries
        try:    
            cursor = conn.cursor()
        except:
            self.status = 2
            return 

        # Execute script and commit changes in database
        try:
            with open(self.init_script, 'r') as f:
                sql_script = f.read()
            cursor.executescript(sql_script)
            conn.commit()
        except:
            self.status = 3
            return 
        
        self.status = 0
        return 


    def __get_connection(self):
        if not os.path.exists(self.path):
            return None
        conn = sqlite3.connect(self.path,check_same_thread=False)
        conn.row_factory = sqlite3.Row  # Allows rows to be accessed as dictionaries
        return conn
    
    def write_data(self,script,data):
        try:
            conn = self.__get_connection()
        except ValueError as e:
            self.status = 10
            return f"[ERROR] Caught an error while getting connection to database ', error: {e}"
        
        try:
            cursor = conn.cursor()
        except ValueError as e:
            conn.close()
            self.status = 10
            return f"[ERROR] Caught an error while getting cursor to database', error: {e}"
        cursor.execute(script, data)
        conn.commit()
        cursor.close()
        conn.close()
        return "[SUCCES] Data has been written into database"


    def read_data(self,query,fetch='one',data=None):
        conn = self.__get_connection()
        cursor = conn.cursor()
        record = None
        try:
            if data:
                cursor.execute(query, (data,))
            else:
                cursor.execute(query)
            
            if fetch == 'one':
                record = cursor.fetchone()
            elif fetch == 'all':
                record = cursor.fetchall()


        except sqlite3.Error as e:
            print("Error while accessing SQLite database:", e)

        finally:
            # Close the connection
            conn.close()
            return record
        
