#pragma once

#include <Windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include "imgui/imgui.h"
#include <string>
#include <vector>
#include <dxgi.h>
#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define CHECK(expr)                                                    \
    if (!(expr))                                                       \
    {                                                                  \
        std::cerr << "Check failed at " << __FILE__ << ":" << __LINE__ << "\n"; \
        std::cerr << "Expression: " << #expr << "\n";                  \
        std::terminate();                                              \
    }

// Helper to print HRESULT errors
inline void LogError(HRESULT hr, const char* msg)
{
	if (FAILED(hr))
	{
		std::cerr << msg << ": HRESULT = " << std::hex << hr << std::endl;
	}
}

struct screen {
	std::int32_t x;
	std::int32_t y;
	std::int32_t width;
	std::int32_t height;

	bool operator==(const screen& screen) const {
		return memcmp(this, &screen, sizeof(screen)) == 0;
	}

	bool operator!=(const screen& screen) const {
		return !(*this == screen);
	}
};

class overlay
{
public:
	static bool render();
	static void move_window(HWND hWnd);
	static bool full_screen(HWND windowHandle);
//	static ImFont* font;
	static ImFont* m_pFont;
private:
	static bool create_device_d3d(HWND hWnd);
	static void cleanup_device_d3d();
	static void create_render_target();
	static void cleanup_render_target();
	static LRESULT WINAPI window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	static ID3D11Device* d3d11Device;
	static ID3D11DeviceContext* d3d11DeviceContext;
	static IDXGISwapChain* dxgiSwapChain;
	static ID3D11RenderTargetView* d3d11RenderTargetView;
	static bool init;
};