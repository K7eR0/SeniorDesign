import socket

# --- Configuration ---
HOST = '0.0.0.0'  # Listen on all available network interfaces
PORT = 8888       # Port should match the one in the ESP32 sketch

def main():
    print(f"Listening for IMU data on {HOST}:{PORT}...")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # Allow address reuse
        s.bind((HOST, PORT))
        s.listen()
        try:
            while True:
                print("Waiting for a connection...")
                conn, addr = s.accept()
                with conn:
                    print(f"Connected by {addr}")
                    while True:
                        data = conn.recv(1024) # Receive up to 1024 bytes
                        if not data:
                            print(f"Connection closed by {addr}")
                            break
                        try:
                            # Decode data and split into values
                            message = data.decode('utf-8').strip()
                            if not message: # Skip empty messages
                                continue
                            print(f"Received: {message}")

                            # Process the IMU data (e.g., split CSV)
                            parts = message.split(',')
                            if len(parts) == 6: # ax, ay, az, gx, gy, gz
                                ax, ay, az, gx, gy, gz = map(float, parts)
                                print(f"Accel: x={ax:.2f}, y={ay:.2f}, z={az:.2f} | Gyro: x={gx:.2f}, y={gy:.2f}, z={gz:.2f}")
                            else:
                                print(f"Received malformed data: {message}")

                        except UnicodeDecodeError:
                            print(f"Received non-UTF-8 data: {data}")
                        except ValueError:
                            print(f"Could not convert data to float: {message}")
                        except Exception as e:
                            print(f"Error processing data: {e}")
        except KeyboardInterrupt:
            print("Server shutting down.")
        finally:
            s.close()

if __name__ == '__main__':
    main()