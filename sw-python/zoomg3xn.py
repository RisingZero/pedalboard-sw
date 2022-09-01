import rtmidi
import threading
from logger import Logger
from enum import Enum

class ZoomG3Xn:

    class _MESSAGE_TYPES(Enum):
        EDITOR_MODE_ON = 1
        EDITOR_MODE_OFF = 2
        TOGGLE = 3

    class _MESSAGE_PARAMS(Enum):
        CHAIN_POSITION = 7
        STATE = 9

    _MESSAGE = {
        _MESSAGE_TYPES.EDITOR_MODE_ON: [0xf0, 0x52, 0x00, 0x6e, 0x50, 0xf7], 
        _MESSAGE_TYPES.EDITOR_MODE_OFF: [0xf0, 0x52, 0x00, 0x6e, 0x51, 0xf7], 
        _MESSAGE_TYPES.TOGGLE: [0xf0, 0x52, 0x00, 0x6e, 0x64, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7] # idx 7 is position in chain
    }

    def __init__(self, port_name = "ZOOM"):
        self.log = Logger('zoomg3xn')
        self.log.warning("Opening MIDI connection to pedalboard...")
        # Create midi interfaces
        self.midiin = rtmidi.MidiIn()
        self.midiout =rtmidi.MidiOut()
        # Allow sysex communication
        self.midiin.ignore_types(sysex=False)

        in_ports = self.midiin.get_ports()
        out_ports = self.midiout.get_ports()

        if not in_ports or not out_ports:
            raise RuntimeError("No midi ports")
        for idx, p in enumerate(in_ports):
            if port_name in p:
                self.midiin.open_port(idx)
                self.log.info("Midi In port ready")
                break
        for idx, p in enumerate(out_ports):
            if port_name in p:
                self.midiout.open_port(idx)
                self.log.info("Midi Out port ready")
                break
        if not self.midiin.is_port_open() or not self.midiout.is_port_open():
            raise RuntimeError("A port wasn't opened")
        
        self.on_receive = None
        self.thread = threading.Thread(target=self._loop, daemon=True)
        self.log.info("Pedalboard connected")


    def _loop(self):
        self.midiout.send_message(self._MESSAGE[self._MESSAGE_TYPES.EDITOR_MODE_ON]) # Edit mode
        self.log.info("Pedalboard thread started.")

        while True:
            if not self.midiin.is_port_open() or not self.midiout.is_port_open():
                self.log.error("Midi ports disconnected, stopping thread")
                break
            msg = self.midiin.get_message()
            if msg:
                bytes_res = bytes(msg[0]).hex()
                self._parse_message(bytes_res)

    def start(self):
        self.log.warning("Starting pedalboard thread...")
        self.thread.start()

    def toggleEffect(self, effect_num, state):
        # Send sysex to activate/deactivate effect
        _msg = self._MESSAGE[self._MESSAGE_TYPES.TOGGLE]
        _msg[self._MESSAGE_PARAMS.CHAIN_POSITION.value] = effect_num
        _msg[self._MESSAGE_PARAMS.STATE.value] = state
        self.midiout.send_message(_msg)

        self.log.info("Toggled {}: {}".format(effect_num, 'ON' if state == 1 else 'OFF'))

    def _parse_message(self, msg: str, **kwargs):
        # parse the midi/sysex message and translate it to <command>:<number> format
        if msg.startswith('f052006e6403'):
            effect_num = int(msg[14:16], 16)
            state = int(msg[18:20], 16)
            _parsed = "{}:{}".format("on" if state == 1 else "off", effect_num)
            self.log.info("Got midi message, {}".format(_parsed))
            if self.on_receive:
                self.on_receive(_parsed, **kwargs)

    def thread_alive(self):
        return self.thread.is_alive()
        
