from flask import Flask, render_template, request, jsonify, send_file
import threading
from database import Database
from DEFINE import *



class HTTPClientHandler:
    def __init__(self):
        self.app = Flask(__name__,static_folder="static",template_folder="templates")
        self.app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0 
        self.devices = Database(path=DATABASE_DEVICES,
                          init_script=None)
        self.measurement = Database(path=DATABASE_MEASUREMENT,
                          init_script=None)
    
        self.logs = Database(path=DATABASE_LOG,
                            init_script=None)

            # Register routes
        self.register_routes()

    def register_routes(self):
        """Define and register all routes for the app."""

        @self.app.route('/')
        def index():
            return render_template('index.html')
        
        @self.app.route('/fetch', methods=["GET"])
        def fetchDate():
            data = self.measurement.read_data(query=SCRIPT_READ_LAST_MEASUREMENT)
            if data is not None:
                data = dict(data)
            log = self.logs.read_data(query=SCRIPT_READ_LAST_LOG)
            if log is not None:
                log = dict(log)
            return jsonify({"data":data,"log":log})

    def run_server(self, host='127.0.0.1', port=5000, debug=True):
        """Run the Flask app."""
        self.app.run(host=host, port=port, debug=debug,use_reloader=False, threaded=True)
