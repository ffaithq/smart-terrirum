import sqlite3
import logging

class DatabaseLogHandler(logging.Handler):
    """
    Custom logging handler to write log records into an SQLite database.
    """
    def __init__(self, db_file):
        super().__init__()
        self.db_file = db_file
        self._setup_database()

    def _setup_database(self):
        # Create a table for logging if it doesn't already exist
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS logs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT DEFAULT CURRENT_TIMESTAMP,
                level TEXT,
                message TEXT
            );
        ''')
        conn.commit()
        conn.close()

    def emit(self, record):
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        try:
            # Use record.msg if there are formatting issues
            message = record.getMessage()
            cursor.execute('''
                INSERT INTO logs (level, message) VALUES (?, ?);
            ''', (record.levelname, message))
            conn.commit()
        except sqlite3.Error as e:
            print(f"Error writing log to database: {e}")
        except Exception as e:
            print(f"Error formatting log message: {e}")
        finally:
            conn.close()

