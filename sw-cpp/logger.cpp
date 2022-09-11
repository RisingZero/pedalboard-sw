#include "logger.hpp"

Logger::Logger(std::string module_name) {
    name = module_name;
}

void Logger::info(std::string message) {
    std::cout << "[" + name + "]  INFO: " << message << std::endl;
}

void Logger::warning(std::string message) {
    std::cout << "[" + name + "]  WARN: " << message << std::endl;
}

void Logger::error(std::string message) {
    std::cout << "[" + name + "] ERROR: " << message << std::endl;
}
