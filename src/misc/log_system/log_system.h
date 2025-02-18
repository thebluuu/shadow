#ifndef LOG_HPP
#define LOG_HPP
// -- [ header guards ] -- \\

#include <string>

namespace RBX {
    class Log_System {
    public:
        static void Log(const std::string& message);
        static void Info(const std::string& message);
        static void Warning(const std::string& message);
        static void Error(const std::string& message);
        void Address(const std::string& message, uint64_t value);
     //   static void Address(const std::string& message, void* address);
     //   void write_to_file(const std::string& logMessage);
    };
}

#endif // LOG_HPP
