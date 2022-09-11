#ifndef ARDUINO_H
#define ARDUINO_H

#include <string>
#include <unistd.h>
#include <thread>
#include "logger.hpp"
#include "serial/serial.h"

class Arduino {
   public:
    Logger* log;
    serial::Serial* tty;
    std::thread* thread;
    void (*callback)(std::string message);

    Arduino(std::string port, int baudrate, void (*on_message)(std::string));
    void start();
    void send(std::string message);
};

#endif