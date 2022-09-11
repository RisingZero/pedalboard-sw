#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

class Logger {
    private:
    std::string name;

    public:
    Logger(std::string name);
    
    void info(std::string message);
    void warning(std::string message);
    void error(std::string message);
};

#endif
