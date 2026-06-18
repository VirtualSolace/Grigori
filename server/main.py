import ssl
import threading
from http.server import HTTPServer
from time import sleep

from MadaraHandler import MadaraHandler

HOST = "localhost"
PORT = 2213

command_queue = []

SELF_DESTRUCT_COMMAND = "terminate"


def run_server(host, port, queue):
    MadaraHandler.queue = queue
    server = HTTPServer((host, port), MadaraHandler)

    # Create SSL context for server
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile="cert.pem", keyfile="key.pem")

    # Wrap server socket
    server.socket = context.wrap_socket(server.socket, server_side=True)

    print(f"Starting HTTPS server @ https://{host}:{port}")
    server.serve_forever()


def input_command():
    command = input("Enter your command: ")
    command_queue.append(command)
    print(f"Command added: {command}")


def add_self_destruct():
    if command_queue and command_queue[-1] != SELF_DESTRUCT_COMMAND:
        command_queue.append(SELF_DESTRUCT_COMMAND)


def menu():
    while True:
        print("\n=== GRIGORI MENU ===")
        print("1 - Add Command")
        print("2 - Add Terminate")
        print("3 - Start Server")
        print("4 - Show Command Queue")
        print("5 - Clear Queue")
        print("6 - Exit")
        choice = input("Choose an option: ")

        if choice == "1":
            input_command()
        elif choice == "2":
            add_self_destruct()
            print("Added Terminate to End of Queue")
        elif choice == "3":
            # Start server in a background thread so menu remains usable
            t = threading.Thread(
                target=run_server, args=(HOST, PORT, command_queue), daemon=True
            )
            t.start()
            sleep(2)
        elif choice == "4":
            print("Current commands in queue:")
            add_self_destruct()
            for i, cmd in enumerate(command_queue, start=1):
                print(f"{i}: {cmd}")
        elif choice == "5":
            command_queue.clear()
            print("Queue cleared...")
        elif choice == "6":
            print("Goodbye...")
            break
        else:
            print("Invalid choice. Select 1-4...")


if __name__ == "__main__":
    menu()
