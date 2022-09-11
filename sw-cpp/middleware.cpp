#include <iostream>
#include <vector>
#include <string>
#include "logger.hpp"
#include "zoomg3xn.hpp"
#include "arduino.hpp"

ZoomG3Xn* zoom;
Arduino* arduino;

void on_zoom_message( double deltatime, std::vector< unsigned char > *message, void *userData )
{
    unsigned char _msg_check[] {0xf0, 0x52, 0x00, 0x6e, 0x64, 0x03};
    for (int i = 0; i < (sizeof(_msg_check)/sizeof(unsigned char)); i++) {
        if (_msg_check[i] != (*message)[i])
            return;
    }
    int effect_num = (int)(*message)[7];
    bool state = ((int)(*message)[9] > 0) ? true : false;

    // Do somenthing with these values, send them to arduino
    if (effect_num < MAX_EFFECT)
        arduino->send(std::string(state ? "on:" : "off:") + std::to_string(effect_num));
    
    return;
}

void on_arduino_message(std::string message) {
    std::string command = message.substr(0, message.find(":"));
    message.erase(0, message.find(":") + std::string(":").length());
    int num_effect = std::stoi(message);

    if (command.compare("on") * command.compare("off") != 0) {
        return;
    }

    zoom->toggle_effect(num_effect, (command.compare("on") == 0) ?  1 : 0);
}

int main(int argc, char **argv) {

    Logger log = Logger("middleware");
    log.info("Starting middleware...");

    arduino = new Arduino(std::string("/dev/cu.usbmodem141201"), 9600, &on_arduino_message);
    arduino->start();

    zoom = new ZoomG3Xn("ZOOM", &on_zoom_message);

    int num_effect = -1;
    while(true) {
        std::cin >> num_effect;
        if (num_effect < MAX_EFFECT  && num_effect > -1) {
            zoom->toggle_effect(num_effect, true);
        }
    };
    return 0;
}