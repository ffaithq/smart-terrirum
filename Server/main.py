from http_server import HTTPClientHandler
from mqtt import MQTTClientHandler
import threading

if __name__ == "__main__":
    mqtt = MQTTClientHandler()
    http = HTTPClientHandler()
    
    http_thread = threading.Thread(target=http.run_server, daemon=True)
    mqtt_thread = threading.Thread(target=mqtt.run_server, daemon=True)

    http_thread.start()
    mqtt_thread.start()

    http_thread.join()
    mqtt_thread.join()