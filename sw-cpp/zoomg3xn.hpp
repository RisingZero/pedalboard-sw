#ifndef ZOOM_H
#define ZOOM_H

#define MAX_EFFECT 7

#include <string>
#include "logger.hpp"
#include "RtMidi.h"

class ZoomG3Xn {
    private:
    Logger* log;
    RtMidiIn* midiin;
    RtMidiOut* midiout;

    public:
    ZoomG3Xn(std::string port_name, RtMidiIn::RtMidiCallback on_message_callback);
    ~ZoomG3Xn();
    void toggle_effect(int effect_num, bool state);
};

#endif
