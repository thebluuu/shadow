#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>
#include "iostream"
#include <vector>
#include <string>
extern uintptr_t virtualaddy;

// Define custom IOCTL codes
#define code_rw CTL_CODE(FILE_DEVICE_UNKNOWN, 0x71, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_ba CTL_CODE(FILE_DEVICE_UNKNOWN, 0x72, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_get_guarded_region CTL_CODE(FILE_DEVICE_UNKNOWN, 0x73, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_security 0x85b3e12

// Structures for interacting with the driver
typedef struct _rw {
    INT32 security;
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
    BOOLEAN write;
} rw, * prw;

typedef struct _ba {
    INT32 security;
    INT32 process_id;
    ULONGLONG* address;
} ba, * pba;

typedef struct _ga {
    INT32 security;
    ULONGLONG* address;
} ga, * pga;

namespace driver {
    extern HANDLE driver_handle;
    extern INT32 process_id;

    // Function declarations
    bool find_driver();
    void read_physical(PVOID address, PVOID buffer, DWORD size);
    void write_physical(PVOID address, PVOID buffer, DWORD size);
    uintptr_t find_image();
    uintptr_t get_guarded_region();
    INT32 find_process(LPCTSTR process_name);

    // Template for reading memory
    template <typename T>
    T read(uint64_t address) {
        T buffer{};
        driver::read_physical((PVOID)address, &buffer, sizeof(T));
      
        return buffer;
    }

    // Template for writing memory
    template <typename T>
    T write(uint64_t address, const T& buffer) {
        driver::write_physical(reinterpret_cast<PVOID>(address), const_cast<PVOID>(static_cast<const void*>(&buffer)), sizeof(T));
     //   std::cout << "Write Completed at: " << address << "Buffered Input: " << buffer << std::endl;
        return buffer;
    }


    template <typename T>
    std::vector<T> read_array(uint64_t address, size_t size) {
        std::vector<T> buffer(size);
        driver::read_physical(reinterpret_cast<PVOID>(address), buffer.data(), size * sizeof(T));
        return buffer;
    }
    static std::string ReadString(std::uint64_t address) {
        std::string string;
        char character = 0;
        int char_size = sizeof(character);
        int offset = 0;

        
        string.reserve(204);

     
        const auto stringLength = read<int>(address + 0x18);
        std::uint64_t stringAddress = stringLength >= 16u ? read<std::uint64_t>(address) : address;

      
        while (offset < 200) {
            character = read<char>(stringAddress + offset);

            if (character == 0) {
                break;
            }

            offset += char_size;
            string.push_back(character);
        }

        return string;
    }

}