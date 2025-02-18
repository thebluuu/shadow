#include "library/web/web.h"
#include "memory.h"
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "misc/globals/globals.hpp"
const std::string LOGIN_FILE = "login.dat";
const char XOR_KEY = 0X73D;

std::string xor_encrypt(const std::string& data) {
    std::string encrypted_data = data;
    for (size_t i = 0; i < data.size(); ++i) {
        encrypted_data[i] = data[i] ^ XOR_KEY;
    }
    return encrypted_data;
}

std::string xor_decrypt(const std::string& encrypted_data) {
    return xor_encrypt(encrypted_data);
}
bool is_debugger_present_advanced() {
    return IsDebuggerPresent();
}

bool are_critical_functions_hooked() {

    FARPROC loadLibraryProc = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    FARPROC getProcAddrProc = GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetProcAddress");

    if (loadLibraryProc == nullptr || getProcAddrProc == nullptr) {
        return true;
    }

    return false;
}

bool is_memory_manipulated() {
    MEMORY_BASIC_INFORMATION mbi;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    LPVOID address = nullptr;

    while (address < sysInfo.lpMaximumApplicationAddress) {
        if (VirtualQuery(address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_WRITECOPY)) {

                return true;
            }
        }
        address = (LPVOID)((char*)address + mbi.RegionSize);
    }
    return false;
}

void protect_process() {
    HANDLE hProcess = GetCurrentProcess();
    DWORD processPriority = HIGH_PRIORITY_CLASS;
    SetPriorityClass(hProcess, processPriority);

    FreeConsole();

    if (is_debugger_present_advanced()) {
        std::cerr << "Debugger detected! Terminating process." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (are_critical_functions_hooked()) {
        std::cerr << "Suspicious hooks detected! Terminating process." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (is_memory_manipulated()) {
        std::cerr << "Memory manipulation detected! Terminating process." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Process protected." << std::endl;
}
void save_token(const std::string& token) {
    std::ofstream file(LOGIN_FILE, std::ios::binary);
    if (file.is_open()) {
        std::string encrypted_token = xor_encrypt(token);
        file.write(encrypted_token.c_str(), encrypted_token.size());
        file.close();
    }
}

bool load_token(std::string& token) {
    std::ifstream file(LOGIN_FILE, std::ios::binary);
    if (file.is_open()) {
        std::string encrypted_token((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
 //       file.
        if (!encrypted_token.empty()) {
            token = xor_decrypt(encrypted_token);
            return true;
        }
    }
    return false;
}

bool debug = globals::debug_info;
bool sigmaboi = false;

//void authinit() {
 //  const std::string version = "1.2";
  //  const std::string program_key = "xrpmiICzuTDAeYHvwhVzqsq674iNAxKPOimTVQf4Eiz";
 //   const std::string api_key = "e5a33517c4021e6610769e14c250f7eb";

 //   s_init(version, program_key, api_key, true);
//    token_example();
//}

int main() {
    createConfigPath();
    utils::console::set_size(12);
    utils::console::enable_ansi();
    SetConsoleTitle("NVIDIA Container");
  //  utils::console::Set_RConsole(true);
  //  authinit();
 RBX::Initializer();
    return 0;
}