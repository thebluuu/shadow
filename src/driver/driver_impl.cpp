#include "driver_impl.hpp"

uintptr_t virtualaddy;
HANDLE driver::driver_handle;
INT32 driver::process_id;

namespace driver {
    bool find_driver() {
        driver_handle = CreateFileW((L"\\\\.\\\IoControl"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
            return false;
        return true;
    }

    void read_physical(PVOID address, PVOID buffer, DWORD size) {
     
        if (!address || !buffer || size == 0) {
            return;  
        }

      
        _rw arguments = {};
        arguments.security = code_security;
        arguments.address = reinterpret_cast<ULONGLONG>(address); 
        arguments.buffer = reinterpret_cast<ULONGLONG>(buffer);
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = FALSE;

      
        DWORD bytesReturned = 0;
        if (!DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, 0, &bytesReturned, nullptr)) {
          
            DWORD error = GetLastError();
           
        }
    }

    void write_physical(PVOID address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };
        arguments.security = code_security;
        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = TRUE;

        DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, 0, nullptr, nullptr);
    }
 
    uintptr_t find_image() {
        uintptr_t image_address = { NULL };
        _ba arguments = { NULL };
        arguments.security = code_security;
        arguments.process_id = process_id;
        arguments.address = (ULONGLONG*)&image_address;
        DeviceIoControl(driver_handle, code_ba, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
        return image_address;
    }

    uintptr_t get_guarded_region() {
        uintptr_t guarded_region_address = { NULL };
        _ga arguments = { NULL };
        arguments.security = code_security;
        arguments.address = (ULONGLONG*)&guarded_region_address;
        DeviceIoControl(driver_handle, code_get_guarded_region, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
        return guarded_region_address;
    }

    INT32 find_process(LPCTSTR process_name) {
        PROCESSENTRY32 pt;
        HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pt.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hsnap, &pt)) {
            do {
                if (!lstrcmpi(pt.szExeFile, process_name)) {
                    CloseHandle(hsnap);
                    process_id = pt.th32ProcessID;
                    return pt.th32ProcessID;
                }
            } while (Process32Next(hsnap, &pt));
        }
        CloseHandle(hsnap);
        return { NULL };
    }
}
