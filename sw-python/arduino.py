import threading
from time import sleep
import serial
import threading
from logger import Logger

class Arduino:
    def __init__(self, port, baudrate, on_receive = None):   
        self.log = Logger('arduino') 
        self.log.warning("Connecting to Arduino...")
        self.tty = serial.Serial(port, baudrate=115200, timeout=.1)
        self.thread = threading.Thread(target=self._loop, args=(on_receive,), daemon=True)

    def send(self, message: bytes):
        self.tty.write(message)

    def _loop(self, callback, **kwargs):
        _exception_count = 0
        while True:
            if not self.tty.is_open:
                self.log.error("Port is closed, stopping thread")
                break
            try:
                command = self.tty.readline()
                if command:
                    callback(command, **kwargs)
            except Exception as e:
                _exception_count += 1
                self.log.error(e)
                if _exception_count > 5:
                    self.log.error("Too many exceptions, stopping thread")
                    break;t
                continue

    def start(self):
        if not self.tty.is_open:
            self.tty.open()
        sleep(2)
        self.send(b"START\n")
        started = False
        while not started:
            self.tty.read_until(b"OK\n")
            started = True
        self.log.info("Arduino serial connection started")
        self.thread.start()

    def thread_alive(self):
        return self.thread.is_alive()
        