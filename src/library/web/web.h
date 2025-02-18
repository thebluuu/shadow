#include <Windows.h>
#include "../../roblox/rbx_engine.h"
#include "thread"
#include <string>
#include <iostream>
#include <Shlobj.h>
#include <filesystem>
#include <fstream>
#include "../../misc/output/console.hpp"
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <wincrypt.h>
#pragma comment(lib, "wininet.lib")

namespace fs = std::filesystem;

std::string fetch_url_content(const std::string& url) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[4096];
    std::string result;

    hInternet = InternetOpen("WinINet Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "InternetOpen failed with error: " << GetLastError() << std::endl;
        return "";
    }

    hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrlA failed with error: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        result.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return result;
}

std::string get_appdata() {
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path) == S_OK) {
        return std::string(path);
    }
    return "";
}

void createConfigPath() {
    std::string appDataPath = get_appdata();
    std::string targetDir = appDataPath + "\\Shadow\\configs";

    try {
        if (!fs::exists(targetDir)) {
            fs::create_directories(targetDir);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
    }
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string get_cpu_id() {
    int CPUInfo[4] = { -1 };
    (CPUInfo, 0);

    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << CPUInfo[0]
        << std::setw(8) << std::setfill('0') << CPUInfo[1]
        << std::setw(8) << std::setfill('0') << CPUInfo[2]
        << std::setw(8) << std::setfill('0') << CPUInfo[3];

    return ss.str();
}

std::string custom_hash(const std::string& input) {
    uint64_t hash = 0;

    for (size_t i = 0; i < input.size(); ++i) {
        hash ^= (input[i] << (i % 64));
        hash *= 16777619;
    }

    std::stringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << hash;
    return ss.str();
}

std::string get_hwid() {
    std::string cpu_id = get_cpu_id();
    return custom_hash(cpu_id);
}
std::string clean_hwid(const std::string& hwid) {
    std::string clean = trim(hwid);
    std::transform(clean.begin(), clean.end(), clean.begin(), ::toupper); // Ensure the case is the same
    return clean;
}

void check_server_auth(const std::string& url, bool IsAuthTrue) {
    if (!IsAuthTrue) {
       
    }

    if (IsAuthTrue) {
        std::string sigma = "https://pastebin.com/g24DdEJB";
        std::string content = fetch_url_content(url);
        std::string hwidtest = fetch_url_content(sigma);
        HWND consoleWindow = GetConsoleWindow();
        SetWindowPos(consoleWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        std::cout << "Checking Auth Server\n" << std::endl;
        content = trim(content);
        std::transform(content.begin(), content.end(), content.begin(), ::toupper);

        if (content == "TRUE") {
            std::cout << "Server returned \n" << content << std::endl;
            std::cout << "\nCalling Init\n" << std::endl;

            std::string hwid = get_hwid();
            std::string cleanHwid = clean_hwid(hwid);
            std::string cleanHwidTest = clean_hwid(hwidtest);

            std::cout << "Raw HWID (local): " << get_cpu_id() << std::endl;
            std::cout << "Raw HWID (server): " << hwidtest << std::endl;

            std::cout << "HWID: " << cleanHwid << std::endl;
            std::cout << "Server HWID: " << cleanHwidTest << std::endl;

            if (cleanHwidTest == cleanHwid) {
                std::cout << "HWID found in DataBase\n";
                RBX::Initializer();
            }
            else {
                std::cout << "HWID not in the whitelist, aborting init.\n";
                std::cout << "Press any key to exit.\n";
                std::cin.get();
            }
        }
        else if (content == "FALSE") {
            std::cout << "Server KillSwitch Enabled\n" << std::endl;
            std::cout << "Press Any Key To Continue\n" << std::endl;
            std::cin.get();
        }
        else {
            std::cout << "Server Info Outdated. Wait 10 minutes\n" << std::endl;
        }
    }
}