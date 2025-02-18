#include "output.hpp"

//<output.cpp>
namespace utils::output 
{
    //
     void infoaddress(const std::string& message, uint64_t value) 
    {
         printf("[\033[0;32minfo\033[0;37m] %s: 0x%llx\n", message.c_str(), value);
     
    }
    //
     void printaddress(const std::string& message, void* value)
     {
         printf("[\033[0;34moutput\033[0;37m] %s: 0x%llx\n", message.c_str(), value);
     }
     void printint(const std::string& message, int value)
     {
         printf("[\033[0;32minfo\033[0;37m] %s: %p\n", message.c_str(), value);
     }
    void print(const std::string& message) 
    {
        printf("[\033[0;34moutput\033[0;37m] %s\n", message.c_str());
    }
    //
      void input(const std::string& message) 
    {
        printf("[\033[0;34minput\033[0;37m] %s\n", message.c_str());
    }
    //
      void inputA(const std::string& prompt)
      {
          std::cout << "[\033[0;34minput\033[0;37m] " << prompt;
          std::string userInput;
          std::getline(std::cin, userInput);
         
      }
      //
    void info(const std::string& message) 
    {
        printf("[\033[0;32minfo\033[0;37m] %s\n", message.c_str());
    }
    //
    void warning(const std::string& message)
    {
        printf("[\033[0;33mwarning\033[0;37m] %s\n", message.c_str());
    }
    //
    void error(const std::string& message)
    {
        printf("[\033[0;31merror\033[0;37m] %s\n", message.c_str());
    }
    //
    void count(const char* message, ...) 
    {  
        va_list args;
        va_start(args, message);
        printf("[\033[0;35mcount\033[0;37m] ");
        vprintf(message, args);
        printf("\n");
        va_end(args);
    }
    //
    void address(const char* message, ...) 
    {  
        va_list args;
        va_start(args, message);
        printf("[\033[0;35maddress\033[0;37m] ");
        vprintf(message, args);
        printf("\n");
        va_end(args);
    }
    //
    void begin(const std::string& message)
    {
        printf("[\033[0;33mbegin\033[0;37m] %s\n", message.c_str());
    }

    void end(const std::string& message)
    {
        printf("[\033[0;33mend\033[0;37m] %s\n", message.c_str());
    }
    //
    void clear_screen()
    {
        system("cls");
    }
    //
}
//<Quiet>