#include "log_system.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>

namespace RBX {
    std::mutex log_mutex;
    const std::string LOG_FILE = "log.txt";

    void write_to_file(const std::string& logMessage) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::ofstream file(LOG_FILE, std::ios::app);
        if (file.is_open()) {
            file << logMessage << std::endl;
            file.close();
        }
    }

    std::string get_time_stamp() {
        std::time_t now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    void Log_System::Log(const std::string& message) {
        std::string logMessage = "[LOG] " + get_time_stamp() + " - " + message;
        std::cout << logMessage << std::endl;
        write_to_file(logMessage);
    }

    void Log_System::Info(const std::string& message) {
        std::string logMessage = "[INFO] " + get_time_stamp() + " - " + message;
        std::cout << "\033[0;32m" << logMessage << "\033[0;37m" << std::endl;
        write_to_file(logMessage);
    }

    void Log_System::Warning(const std::string& message) {
        std::string logMessage = "[WARNING] " + get_time_stamp() + " - " + message;
        std::cout << "\033[0;33m" << logMessage << "\033[0;37m" << std::endl;
        write_to_file(logMessage);
    }

    void Log_System::Error(const std::string& message) {
        std::string logMessage = "[ERROR] " + get_time_stamp() + " - " + message;
        std::cout << "\033[0;31m" << logMessage << "\033[0;37m" << std::endl;
        write_to_file(logMessage);
    }

    void Log_System::Address(const std::string& message, uint64_t value) {
        std::string logMessage = "[ADDRESS] " + get_time_stamp() + " - " + message + ": 0x" + std::to_string(value);
        std::cout << "\033[0;35m" << logMessage << "\033[0;37m" << std::endl;
        write_to_file(logMessage);
    }
}
