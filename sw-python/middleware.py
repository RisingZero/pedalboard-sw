from time import sleep
from zoomg3xn import ZoomG3Xn
from arduino import Arduino
from logger import Logger

# socat -d -d pty,raw,echo=0 pty,raw,echo=0
# echo 'on:1' > /dev/ttys001
# /dev/cu.usbmodem141201 || ARDUINO

log = Logger('middleware')

log.info("Starting middleware...")

try:
    zoom = ZoomG3Xn(port_name="Midi")

    def on_arduino_message(msg: bytes):
        command = msg.decode().strip().split(':')
        if command[0] in ['on','off']:
            zoom.toggleEffect(int(command[1]), 1 if command[0] == 'on' else 0)

    arduino = Arduino(port='/dev/cu.usbmodem141201', baudrate=9600, on_receive=on_arduino_message)
    arduino.start()

    def on_zoom_message(msg: str):
        command = msg.strip()
        if command.split(':')[0] in ['on','off']:
            arduino.send(command.encode())

    zoom.on_receive = on_zoom_message
    zoom.start()

except Exception as err:
    log.error(err)
    exit(1)

while True:
    if not arduino.thread_alive() or not zoom.thread_alive():
        log.error("One of the threads stopped, stopping middleware...")
        break
    sleep(3)

