#include "zoomg3xn.hpp"

enum MESSAGE_PARAMS {
    CHAIN_POSITION = 7,
    STATE = 9
};

const unsigned char MESSAGE_EDITOR_MODE_ON[] = {0xf0, 0x52, 0x00, 0x6e, 0x50, 0xf7};
const unsigned char MESSAGE_EDITOR_MODE_OFF[] = {0xf0, 0x52, 0x00, 0x6e, 0x51, 0xf7};
unsigned char MESSAGE_TOGGLE[] = {0xf0, 0x52, 0x00, 0x6e, 0x64, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7}; // params at enum positions

ZoomG3Xn::ZoomG3Xn(std::string port_name, RtMidiIn::RtMidiCallback on_message_callback) {
    log = new Logger("zoomg3xn");
    try {
        midiin = new RtMidiIn();
        midiout = new RtMidiOut();
    } catch ( RtMidiError &error) {
        log->error(error.getMessage());
        exit( EXIT_FAILURE );
    }

    // Setup ignoreTypes
    midiin->ignoreTypes(false, true, true);

    log->info("Starting MIDI ports configurations");

    unsigned int portCount = midiin->getPortCount();
    if (portCount == 0) {
        log->error("No MIDI input ports");
        exit( EXIT_FAILURE );
    }
    for (unsigned int i = 0; i < portCount; i++) {
        try {
            std::string portName = midiin->getPortName(i);
            if (portName.find(port_name) != std::string::npos) {
                midiin->openPort(i);
                midiin->setCallback(on_message_callback);
                break;
            }
        } catch ( RtMidiError &error ) {
            log->error(error.getMessage());
            exit( EXIT_FAILURE );
        }
    }
    if (!midiin->isPortOpen()) {
        log->error("MIDI input port is not open");
        exit( EXIT_FAILURE );
    }
    log->info("MIDI input port ready");

    portCount = midiout->getPortCount();
    if (portCount == 0) {
        log->error("No MIDI output ports");
        exit( EXIT_FAILURE );
    }
    for (unsigned int i = 0; i < portCount; i++) {
        try {
            std::string portName = midiout->getPortName(i);
            if (portName.find(port_name) != std::string::npos) {
                midiout->openPort(i);
                break;
            }
        } catch ( RtMidiError &error ) {
            log->error(error.getMessage());
            exit( EXIT_FAILURE );
        }
    }
    if (!midiout->isPortOpen()) {
        log->error("MIDI output port is not open");
        exit( EXIT_FAILURE );
    }
    log->info("MIDI output port ready");

    std::vector<unsigned char> start(MESSAGE_EDITOR_MODE_ON, MESSAGE_EDITOR_MODE_ON + (sizeof(MESSAGE_EDITOR_MODE_ON)/sizeof(unsigned char)));
    try {
        midiout->sendMessage(&start);
    } catch ( RtMidiError &error ) {
        log->error(error.getMessage());
        exit( EXIT_FAILURE );
    }
    log->info("Zoom pedalboard connected");
}

ZoomG3Xn::~ZoomG3Xn() {
    midiin->closePort();
    midiout->closePort();

    delete log;
    delete midiin;
    delete midiout;
}

void ZoomG3Xn::toggle_effect(int num_effect, bool state) {
    std::vector<unsigned char> _msg(MESSAGE_TOGGLE, MESSAGE_TOGGLE + (sizeof(MESSAGE_TOGGLE)/sizeof(unsigned char)));

    _msg[MESSAGE_PARAMS::CHAIN_POSITION] = (unsigned char)num_effect; 
    _msg[MESSAGE_PARAMS::STATE] = state ? 0x1 : 0x0;

    try {
        midiout->sendMessage(&_msg);
    } catch ( RtMidiError &error ) {
        log->error(error.getMessage());
        if (!midiout->isPortOpen())
            exit( EXIT_FAILURE );
    }
}
