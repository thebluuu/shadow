#pragma once
#include <string>
#include <iostream>
#include <cstdarg>

//<output.hpp>
namespace utils
{
    //
    namespace output
    {
        //
        void printint(const std::string& message, int value);
        void printaddress(const std::string& message, void* value);
        void inputA(const std::string& prompt);
        void input(const std::string& message);
        void infoaddress(const std::string& message, uint64_t value);
        void print(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void count(const char* message, ...);  
        void address(const char* message, ...); 
        void begin(const std::string& message);
        void end(const std::string& message);
        void clear_screen();
        //
    }
    //
}
//<Quiet>