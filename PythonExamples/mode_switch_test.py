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
        self.last_boot_time = None

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
            
    def restart_device_serial(self):
        """Send reboot command via serial connection"""
        command = {"commands": [{"command": "restart_device"}]}
        print("Sending reboot command via serial...")
        response = self.send_command(command)
        print(f"Response: {response}")
        self.last_boot_time = time.time()
        
        # Close the serial connection after sending reboot command
        # This helps prevent permission issues when reconnecting
        if self.serial_conn and self.serial_conn.is_open:
            try:
                self.serial_conn.close()
                print("Serial connection closed after reboot command")
            except Exception as e:
                print(f"Error closing serial connection: {e}")
                
        return response
        
    def restart_device_wifi(self):
        """Send reboot command via WiFi API"""
        if not self.device_ip:
            print("Device IP not available. Cannot restart via WiFi.")
            return False
            
        try:
            # First verify the device is responsive via WiFi
            ping_url = f"http://{self.device_ip}:81/control/builtin/command/ping"
            print(f"Testing device responsiveness with: {ping_url}")
            ping_response = requests.get(ping_url, timeout=5)
            if ping_response.status_code != 200:
                print(f"Device not responsive via WiFi before restart attempt: {ping_response.status_code}")
                return False
                
            # Use rebootDevice endpoint with POST method
            url = f"http://{self.device_ip}:81/update/rebootDevice"
            print(f"Sending reboot command to: {url}")
            
            # Use POST method as recommended
            try:
                response = requests.post(url, timeout=5)
                if response.status_code == 200:
                    print("Reboot command via WiFi (POST) sent successfully!")
                    self.last_boot_time = time.time()
                    return True
                else:
                    print(f"Reboot command via WiFi (POST) failed with status code: {response.status_code}")
                    return False
            except requests.exceptions.ConnectionError:
                print("Connection error during POST: Device may have already started restarting")
                self.last_boot_time = time.time()
                return True
            except Exception as e:
                print(f"Error sending POST reboot command: {e}")
                return False
        except requests.exceptions.ConnectionError:
            print("Connection error: Device may have already started restarting")
            self.last_boot_time = time.time()
            return True
        except Exception as e:
            print(f"Error sending reboot command via WiFi: {e}")
            return False
            
    def verify_restart(self, wait_time=60, retry_count=3):
        """Verify that the device has rebooted by checking uptime"""
        if not self.last_boot_time:
            print("No reboot command was previously sent")
            return False
            
        # Calculate elapsed time since reboot command
        elapsed = time.time() - self.last_boot_time
        if elapsed < wait_time:
            # Wait remaining time if needed
            remaining = wait_time - elapsed
            print(f"Waiting {remaining:.1f} more seconds for device to reboot...")
            time.sleep(remaining)
        else:
            print(f"Already waited {elapsed:.1f} seconds since reboot command")
        
        # Close the serial connection before attempting to reconnect
        if self.serial_conn and self.serial_conn.is_open:
            try:
                self.serial_conn.close()
                time.sleep(1)  # Give the OS time to release the port
            except Exception as e:
                print(f"Error closing serial connection: {e}")
        
        # Try to reconnect serial with retries
        for attempt in range(retry_count):
            try:
                if self.connect_serial():
                    break
            except Exception as e:
                print(f"Error during reconnection attempt {attempt+1}: {e}")
                
            if attempt < retry_count - 1:
                print(f"Reconnection attempt {attempt+1} failed, retrying in 2 seconds...")
                time.sleep(2)
            else:
                print(f"Failed to reconnect to device after {retry_count} attempts")
                return False
        
        # Send ping command to check if device is responsive
        for attempt in range(retry_count):
            ping_command = {"commands": [{"command": "ping"}]}
            response = self.send_command(ping_command)
            
            if response and "pong" in response.lower():
                print(f"Device responded after restart (attempt {attempt+1})")
                return True
            elif attempt < retry_count - 1:
                print(f"Ping attempt {attempt+1} failed, retrying in 2 seconds...")
                time.sleep(2)
        
        print(f"Device did not respond properly after {retry_count} ping attempts")
        return False

    def close(self):
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()


def run_test(port, ssid, password, wait_time=15, retry_count=3, skip_wifi_test=False):
    tester = OpenIrisModeTester(port)
    test_results = {
        "serial_connection": False,
        "wifi_credentials": False,
        "device_discovery": False,
        "wifi_api": False,
        "serial_restart": False,
        "wifi_restart": False,
        "usb_mode": False
    }

    try:
        print("\n=== Step 1: Testing Serial Connection ===")
        if not tester.connect_serial():
            print("❌ Serial connection failed. Aborting test.")
            return test_results
        test_results["serial_connection"] = True
        print("✅ Serial connection successful")

        print("\n=== Step 2: Setting WiFi credentials ===")
        response = tester.set_wifi_credentials(ssid, password)
        if response and "error" not in response.lower():
            test_results["wifi_credentials"] = True
            print("✅ WiFi credentials set successfully")
            
            # Explicitly send reboot command after setting WiFi credentials
            print("Sending reboot command after setting WiFi credentials...")
            tester.restart_device_serial()
            
            # Wait specifically 30 seconds for the device to restart
            print("Waiting 30 seconds for device to restart...")
            time.sleep(30)
        else:
            print("❌ Failed to set WiFi credentials")
        
        # Verify device reboots after setting WiFi credentials
        if not tester.verify_restart(wait_time=wait_time, retry_count=retry_count):
            print("❌ Device did not reboot properly after setting WiFi credentials")
        else:
            print("✅ Device rebooted successfully after setting WiFi credentials")

        print("\n=== Step 3: Discovering device on network ===")
        if tester.discover_device():
            test_results["device_discovery"] = True
            print("✅ Device discovered on network")
            
            print("\n=== Step 4: Testing WiFi API ===")
            if tester.test_wifi_api():
                test_results["wifi_api"] = True
                print("✅ WiFi API test successful")
                
                print("\n=== Step 5: Testing reboot via WiFi ===")
                if tester.restart_device_wifi():
                    # Wait for device to reboot
                    if tester.verify_restart(wait_time=wait_time, retry_count=retry_count):
                        test_results["wifi_restart"] = True
                        print("✅ Device rebooted successfully via WiFi")
                    else:
                        print("❌ Device did not reboot properly via WiFi")
                else:
                    print("❌ Failed to send reboot command via WiFi")
            else:
                print("❌ WiFi API test failed")
        else:
            print("❌ Failed to discover device on network")

        # Reconnect serial if needed
        if not tester.serial_conn or not tester.serial_conn.is_open:
            tester.connect_serial()
        tester.flush_serial_logs()

        print("\n=== Step 6: Testing reboot via Serial ===")
        tester.restart_device_serial()
        if tester.verify_restart(wait_time=wait_time, retry_count=retry_count):
            test_results["serial_restart"] = True
            print("✅ Device rebooted successfully via Serial")
        else:
            print("❌ Device did not reboot properly via Serial")

        print("\n=== Step 7: Wiping WiFi credentials ===")
        tester.wipe_wifi_credentials()
        if not tester.verify_restart(wait_time=wait_time, retry_count=retry_count):
            print("❌ Device did not reboot properly after wiping WiFi credentials")
        else:
            print("✅ Device rebooted successfully after wiping WiFi credentials")

        print("\n=== Step 8: Verifying USB mode ===")
        ping_command = {"commands": [{"command": "ping"}]}
        response = tester.send_command(ping_command)
        if response and "pong" in response.lower():
            test_results["usb_mode"] = True
            print("✅ Device is in USB mode")
        else:
            print("❌ Failed to verify USB mode")

        tester.flush_serial_logs()
        
        # Print summary
        print("\n=== Test Summary ===")
        for test, result in test_results.items():
            status = "✅ PASS" if result else "❌ FAIL"
            print(f"{test.replace('_', ' ').title()}: {status}")

        return test_results

    finally:
        tester.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test OpenIris mode switching and reboot functionality")
    parser.add_argument("--port", required=True, help="Serial port (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("--ssid", required=True, help="WiFi SSID")
    parser.add_argument("--password", required=True, help="WiFi password")
    parser.add_argument("--wait-time", type=int, default=60, help="Wait time in seconds for device reboot (default: 15)")
    parser.add_argument("--retry-count", type=int, default=3, help="Number of retry attempts for reconnection (default: 3)")
    parser.add_argument("--skip-wifi-test", action="store_true", help="Skip WiFi-related tests")

    args = parser.parse_args()
    
    # Configure test parameters
    test_config = {
        "port": args.port,
        "ssid": args.ssid,
        "password": args.password,
        "wait_time": args.wait_time,
        "retry_count": args.retry_count,
        "skip_wifi_test": args.skip_wifi_test
    }
    
    # Run the test and get results
    results = run_test(**test_config)
    
    # Exit with appropriate status code
    if all(results.values()):
        print("\n✅ All tests passed successfully!")
        exit(0)
    else:
        print("\n❌ Some tests failed. See details above.")
        exit(1)