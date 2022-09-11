#include "arduino.hpp"

void _loop(Arduino* arduino) {
    int _exception_count = 0;
    std::string recvd;
    while (true) {
        if (!arduino->tty->isOpen()) {
            arduino->log->error("Arduino port is closed, stopping thread");
            break;
        }
        try {
            if (arduino->tty->available()) {
                recvd = arduino->tty->readline();
                arduino->callback(recvd);
            }
        } catch ( std::exception &error ) {
            arduino->log->error("Error reading from Arduino");
            arduino->log->error(error.what());
            _exception_count++;
            if (_exception_count > 5) {
                arduino->log->error("Too many exceptions, stopping thread");
                break;
            }
            continue;
        }
    }
}

Arduino::Arduino(std::string port, int baudrate, void (*on_message)(std::string)) {
    log = new Logger("arduino");
    log->info("Connecting to Arduino...");

    try {
        tty = new serial::Serial(port, baudrate, serial::Timeout::simpleTimeout(100));
    } catch ( std::exception &error ) {
        log->error("Error connecting to Arduino");
        log->error(error.what());
        exit( EXIT_FAILURE );
    }
    callback = on_message;
}

void Arduino::start() {
    try {
        if (!tty->isOpen()) {
            tty->open();
        }

        sleep(2);

        tty->write("START\n");
        bool started = false;
        while (!started) {
            if (tty->available()) {
                std::string recv = tty->readline();
                if (recv.find("OK") != std::string::npos)
                    started = true;
            }
        }
        tty->flushInput();
        log->info("Arduino serial connection established");

        thread = new std::thread(_loop, this);

    } catch ( std::exception &error ) {
        log->error("Error connecting to Arduino");
        log->error(error.what());
        exit( EXIT_FAILURE );
    }
}

void Arduino::send(std::string message) {
    if (tty->isOpen()) {
        tty->write(message);
    } else {
        log->error("Error connecting to Arduino");
        exit( EXIT_FAILURE );
    }
}
