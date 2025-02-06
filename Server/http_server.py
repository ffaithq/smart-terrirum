from flask import Flask, render_template, request, jsonify, send_file,session,redirect,url_for
import json
import threading
import os
import time
from database import Database
from DEFINE import *
import bcrypt
from datetime import timedelta
 



class HTTPClientHandler:
    def __init__(self):
        self.app = Flask(__name__,static_folder="static",template_folder="templates")
        self.app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0 
        self.app.secret_key = "some_secret_key"
        self.app.permanent_session_lifetime = timedelta(minutes=2)
        self.devices = Database(path=DATABASE_DEVICES,
                          init_script=None)
        self.measurement = Database(path=DATABASE_MEASUREMENT,
                          init_script=None)
        self.logs = Database(path=DATABASE_LOG,
                            init_script=None)
        self.setpoints_db = Database(path=DATABASE_SETPOINTS,
                    init_script=INIT_SCRIPT_SETPOINTS)
        self.users_db = Database(path=DATABASE_USERS,
                    init_script=INIT_SCRIPT_USERS)
        
        if not self.__search_for_user(ADMIN_USERNAME):
            self.__create_user(username=ADMIN_USERNAME,
                                    plain_password=ADMIN_PASSWORD,
                                    usertype='admin')
        self.__register_routes()

    def __file_extension(self,filename:str) -> str:
        """Return file extension"""
        return filename.rsplit('.', 1)[1].lower()
    
    def __create_user(self, username, plain_password,usertype):
        # 1) Generate salt and hashed password
        password_bytes = plain_password.encode("utf-8")
        salt = bcrypt.gensalt()  # random salt
        hashed = bcrypt.hashpw(password_bytes, salt)
        self.users_db.write_data(script=SCRIPT_INSERT_USER,data=(username, hashed.decode("utf-8"),usertype,))

    def __search_for_user(self, username:str):
        row = self.users_db.read_data(query=SCRITP_SEARCH_FOR_USER,data=(username,))

        if row is not None:
            return True
        
        return False



    def __authenticate_user(self, username:str, plain_password:str):
        row = self.users_db.read_data(query=SCRITP_SEARCH_FOR_USER,data=(username,))

        if row is None:
            print("No such user.")
            return False

        user_id, stored_hash, user_type = row
        # Check the password
        if bcrypt.checkpw(plain_password.encode("utf-8"), stored_hash.encode("utf-8")):
            print(f"User #{user_id} authenticated successfully.")
            session["logged_in"] = True
            session["user_id"] = user_id
            session["user_type"] = user_type
            session.permanent = True
            return True
        else:
            print("Wrong password.")
            return False



    def __register_routes(self):
        """Define and register all routes for the app."""

        @self.app.route('/')
        def index():
            """Return index.html"""
            return render_template('index.html')
        
        @self.app.route('/registration')
        def registration():
            if session.get('user_type') is None and session.get("user_type") != 'admin':
                return redirect("/denied"),403
            
            """Return register.html"""
            return render_template('registration.html')
        
        @self.app.route('/authentication')
        def authentication():
            """Return login.html"""
            return render_template('login.html')

        @self.app.route('/denied')
        def denied():
            """Return 403.html"""
            return render_template('403.html'),403
        
        @self.app.route('/register', methods=['POST'])
        def register():
            if session.get('user_type') is None and session.get("user_type") != 'admin':
                return redirect("/denied"),403
        
            username = request.json.get('username')
            password = request.json.get('password')
            usertype = request.json.get('usertype')

            self.__create_user(username=username,
                               plain_password=password,
                               usertype=usertype)
            
            return jsonify({"message": "User registered successfully!"})
        

        @self.app.route("/login", methods = ['POST'])
        def login():
            username = request.json.get('username')
            password = request.json.get('password')

            succes = self.__authenticate_user(username=username,plain_password=password)

            """Login to private"""
            if succes:
                return redirect(url_for("setting"))
            else:
                return jsonify({"message":"Wrong username or password"})    
         
        @self.app.route("/logout")
        def logout():
            session.pop("logged_in", None)
            return "Logged out successfully." 
        
        @self.app.route('/fetch', methods=["GET"])
        def fetchDate():
            data = self.measurement.read_data(query=SCRIPT_READ_LAST_MEASUREMENT)
            if data is not None:
                data = dict(data)
            log = self.logs.read_data(query=SCRIPT_READ_LAST_LOG)
            if log is not None:
                log = dict(log)
            return jsonify({"data":data,"log":log})
        
        @self.app.route('/setting')
        def setting():
            if not session.get("logged_in"):
                return "Access Denied. Please log in.", 403
            return render_template('setting.html')
        
        @self.app.route('/setpoints/add_setpoint', methods=['POST'])
        def upload_file():
            if not session.get("logged_in"):
                return "Access Denied. Please log in.", 403
            # Get the image file
            if 'image' not in request.files:
                return jsonify({'error': 'No image file provided'}), 400
            
            image = request.files['image']
            try:
                image_extension = self.__file_extension(image.filename)
            except Exception as e:
                return jsonify({'error':f'Issue with file extension: {e}'}),400
            
            if image and image_extension in ALLOWED_EXTENSION:
                    
                try:
                    image_path = os.path.join(UPLOAD_PATH,f"{time.strftime('%Y_%m_%d_%H_%M_%S', time.localtime())}.{image_extension}")
                    image_path_db = os.path.join(REAL_PATH,f"{time.strftime('%Y_%m_%d_%H_%M_%S', time.localtime())}.{image_extension}")
                    image.save(image_path)

                    json_data = request.form.get('json')
                    if json_data:
                        # Parse the JSON string into a Python dictionary
                        data_dict = json.loads(json_data)
                        
                        # Access the individual items in the dictionary
                        name = data_dict.get('name')
                        try:
                            waterlevel = float(data_dict.get('waterlevel', 0))  # Default to 0 if missing or invalid
                        except ValueError:
                            waterlevel = None  # Set to None if conversion fails

                        try:
                            uvlevel = float(data_dict.get('uvlevel', 0))  # Default to 0 if missing or invalid
                        except ValueError:
                            uvlevel = None  # Set to None if conversion fails


                        try:
                            self.setpoints_db.write_data(SCRIPT_INSERT_SETPOINS,(name,waterlevel,uvlevel,image_path_db))
                            return jsonify({'message': 'File and data received successfully'})
                        except Exception as e:
                            return jsonify({'message': f'Error: {e}'}), 400
                
                except Exception as e:
                    return jsonify({'message': f'Error: {e}'})


        @self.app.route("/setpoints/image",methods=['GET'])
        def serve_setpoint_image():
            """
            Lookup the image filename from DB, then read the file from disk and serve it.
            """
            if not session.get("logged_in"):
                return "Access Denied. Please log in.", 403
            setpoint_id = request.args.get("setpoint_id", type=int,default=-1)
            image_path = self.setpoints_db.read_data(query=SCRIPT_READ_IMAGE_SETPOINTS,data=(setpoint_id))

            if image_path is None:
                return "Image not found", 404

            image_filepath = image_path[0]  # e.g. "images/myimage.png" or an absolute path
            for extenstion in ALLOWED_EXTENSION:
                if self.__file_extension(image_filepath) == extenstion:
                    try:
                        return send_file(image_filepath, mimetype=f"image/{extenstion}")
                    except FileNotFoundError:
                        return "File not found on server", 404

        @self.app.route("/setpoints/delete", methods=["DELETE"])
        def delete_item():
            if not session.get("logged_in"):
                return "Access Denied. Please log in.", 403
            setpoint_id = request.args.get("setpoint_id", type=int,default=-1)
            row = self.setpoints_db.delete_data(query=SCRIPT_DELETE_SETPOINT,data=(setpoint_id))
            if row == 0:
                return jsonify({"error": f"No record found with id={setpoint_id}."}), 404

            return jsonify({"message": f"Record with id={setpoint_id} deleted successfully."}), 200

        @self.app.route("/setpoints/activate")
        def activate():
            if not session.get("logged_in"):
                return "Access Denied. Please log in.", 403
            setpoint_id = request.args.get("setpoint_id", type=int,default=-1)
            try:
                self.setpoints_db.update_data(query=SCRIPT_DEACTIVATE_ALL)
            except:
                return jsonify({"error": f"Can not deactivate all setpoints."}), 500 
            try:
                self.setpoints_db.update_data(query=SCRIPT_ACTIVATE_ONE,data=(setpoint_id,))
            except:
                return jsonify({"error": f"Can not activate setpoint with id={setpoint_id}."}), 500
            
            return jsonify({"error": f"Setpoint with id={setpoint_id} was activated."}), 200

        @self.app.route("/setpoints/update", methods = ["GET"])
        def update_value():
            if not session.get("logged_in"):
                return "Access Denied. Please log in.", 403
            
            setpoint_id = request.args.get("setpoint_id", type=int,default=None)
            name = request.args.get("name", type=str,default=None)
            waterlevel = request.args.get("waterlevel", type=int,default=None)
            uvlevel = request.args.get("uvlevel", type=int,default=None)

            if setpoint_id is None:
                return jsonify({"error": "setpoint id is Empty"}), 300
            
            if name is not None:
                try:
                    self.setpoints_db.update_data(query=SCRIPT_UPDATE_NAME,data=(name,setpoint_id))
                except:
                    return jsonify({"error": f"Can not change name for setpoint with id={setpoint_id}."}), 500
                
            if waterlevel is not None:
                try:
                    self.setpoints_db.update_data(query=SCRIPT_UPDATE_WATERLEVEL,data=(waterlevel,setpoint_id))
                except:
                    return jsonify({"error": f"Can not change waterlevel for setpoint with id={setpoint_id}."}), 500
                
            if uvlevel is not None:
                try:
                    self.setpoints_db.update_data(query=SCRIPT_UPDATE_UVLEVEL,data=(uvlevel,setpoint_id))
                except:
                    return jsonify({"error": f"Can not change uvlevel setpoint with id={setpoint_id}."}), 500
                
            return jsonify({"message":"Everything is done"}),200

        @self.app.route('/setpoints/fetch', methods=['GET'])
        def get_items():

            items = self.setpoints_db.read_data(SCRIPT_READ_SETPOINTS_DATABASE,fetch='all')

            # Convert the items into a list of dictionaries for JSON response
            items_list = []
            for item in items:
                items_list.append({
                    'id': item[0],
                    'name': item[1],
                    'waterlevel': item[2],
                    'uvlevel': item[3],
                    'image_filename': item[4],
                    "activated":item[5]
                })
            
            # Return the items as a JSON response
            return jsonify(items_list)
        
    def run_server(self, host='127.0.0.1', port=5000, debug=True):
        """Run the Flask app."""
        self.app.run(host=host, port=port, debug=debug,use_reloader=False, threaded=True)
