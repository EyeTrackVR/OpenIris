#!/usr/bin/env python3
import serial
import json
import time
import requests
import argparse
import socket

class OpenIrisModeTester:
    def __init__(self, port, baudrate=115200, timeout=5):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial_conn = None
        self.device_ip = None
        self.device_port = None

    def connect_serial(self):
        try:
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=self.timeout)
            self.flush_serial_logs()
            print(f"Connected to {self.port} at {self.baudrate} baud")
            return True
        except Exception as e:
            print(f"Error connecting to serial port: {e}")
            return False

    def flush_serial_logs(self):
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.reset_input_buffer()
            self.serial_conn.reset_output_buffer()
            print("Serial logs flushed.")

    def send_command(self, command_obj):
        if not self.serial_conn:
            print("Serial connection not established")
            return False

        try:
            command_json = json.dumps(command_obj)
            self.serial_conn.write(command_json.encode() + b'\n')
            time.sleep(0.5)

            response = ""
            start_time = time.time()
            while (time.time() - start_time) < self.timeout:
                if self.serial_conn.in_waiting:
                    line = self.serial_conn.readline().decode('utf-8', errors='replace').strip()
                    if line:
                        response += line
                        try:
                            json.loads(response)
                            return response
                        except json.JSONDecodeError:
                            pass
                time.sleep(0.1)

            return response if response else None
        except Exception as e:
            print(f"Error sending command: {e}")
            return None

    def set_wifi_credentials(self, ssid, password):
        command = {"commands": [{"command": "set_wifi", "data": {"ssid": ssid, "password": password, "network_name": "main"}}]}
        print("Sending WiFi credentials...")
        response = self.send_command(command)
        print(f"Response: {response}")
        return response

    def wipe_wifi_credentials(self):
        command = {"commands": [{"command": "wipe_wifi_creds"}]}
        print("Wiping WiFi credentials...")
        response = self.send_command(command)
        print(f"Response: {response}")
        return response

    def discover_device(self, hostname="openiristracker.local"):
        try:
            self.device_ip = socket.gethostbyname(hostname)
            self.device_port = 80
            print(f"Device found at {self.device_ip}:{self.device_port}")
            return True
        except socket.error as e:
            print(f"Hostname resolution failed: {e}")
            return False

    def test_wifi_api(self):
        if not self.device_ip:
            print("Device IP not available. Discover device first.")
            return False

        try:
            url = f"http://{self.device_ip}:81/control/builtin/command/ping"
            response = requests.get(url, timeout=5)
            if response.status_code == 200:
                print("WiFi API test successful!")
                return True
            else:
                print(f"WiFi API test failed with status code: {response.status_code}")
                return False
        except Exception as e:
            print(f"Error testing WiFi API: {e}")
            return False

    def close(self):
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()


def run_test(port, ssid, password):
    tester = OpenIrisModeTester(port)

    try:
        if not tester.connect_serial():
            return

        print("\n=== Step 1: Setting WiFi credentials ===")
        tester.set_wifi_credentials(ssid, password)
        print("Device will restart. Waiting 10 seconds...")
        time.sleep(10)

        tester.connect_serial()
        tester.flush_serial_logs()

        print("\n=== Step 2: Discovering device on network ===")
        if tester.discover_device():
            print("\n=== Step 3: Testing WiFi API ===")
            tester.test_wifi_api()

            tester.flush_serial_logs()

            print("\n=== Step 4: Wiping WiFi credentials ===")
            tester.wipe_wifi_credentials()
            print("Device will restart in USB mode. Waiting 10 seconds...")
            time.sleep(10)

            tester.connect_serial()
            tester.flush_serial_logs()

            print("\n=== Step 5: Verifying USB mode ===")
            ping_command = {"commands": [{"command": "ping"}]}
            print("Device is most likely booting, waiting 10 seconds...")
            time.sleep(10)

            response = tester.send_command(ping_command)
            print(f"Response: {response}")

            tester.flush_serial_logs()

    finally:
        tester.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test OpenIris mode switching functionality")
    parser.add_argument("--port", required=True, help="Serial port (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("--ssid", required=True, help="WiFi SSID")
    parser.add_argument("--password", required=True, help="WiFi password")

    args = parser.parse_args()
    run_test(args.port, args.ssid, args.password)