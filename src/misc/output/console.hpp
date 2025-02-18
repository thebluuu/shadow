#pragma once
#include <windows.h>

//<console.hpp>
namespace utils
{
    //
    namespace console
    {
        //
        void set_size(int size)
        {
            CONSOLE_FONT_INFOEX cfi;
            cfi.cbSize = sizeof(cfi);
            cfi.nFont = 0;
            cfi.dwFontSize.X = 0;
            cfi.dwFontSize.Y = size;
            cfi.FontFamily = FF_DONTCARE;
            cfi.FontWeight = FW_NORMAL;
            wcscpy_s(cfi.FaceName, L"Consolas");
            //
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
        }
        void Set_RConsole(bool answer)
        {
            if (answer) {
                HWND consoleWindow = GetConsoleWindow();
                SetWindowPos(consoleWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
            else {
                printf("RConsole Set To False");
            }
          

        }
        //
        void hide_console()
        {
            ShowWindow(GetConsoleWindow(), SW_HIDE);
        }
        //
        void show_console()
        {
            ShowWindow(GetConsoleWindow(), SW_SHOW);
        }
        //
        void enable_ansi()
        {
            HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dw_mode = 0;
            GetConsoleMode(console, &dw_mode);
            dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(console, dw_mode);
        }
    }
    //
}
//<Quiet>