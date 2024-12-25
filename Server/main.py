from flask import Flask,request,jsonify,render_template
import sqlite3
import threading

app = Flask(__name__, static_folder="static", template_folder="templates")

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/todb", method=['POST'])
def saveToDb():
    #TODO create function to save request
    pass

@app.route('/fromDb',method=['GET'])
def readToDb():
    #TODO read frame of db
    pass

@app.route('/sensors',method=['GET'])
def readToDb():
    #TODO return sensors
    pass

@app.route('/setpoints',method=['GET'])
def readToDb():
    #TODO return setpoints
    pass


def run_server():
    app.run(host="0.0.0.0", port=8000, debug=False)


if __name__ == "__main__":
    server_thread = threading.Thread(target=run_server, daemon=True)
    server_thread.start()
    server_thread.join()