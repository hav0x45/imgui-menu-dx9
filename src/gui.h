#pragma once
#include <d3d9.h>

namespace MyGui {

    // Show menu bool
    inline bool open = true;

    // Menu setup bool
    inline bool setup = false;

    // WinAPI related
    inline HWND window = nullptr;
    inline WNDCLASSEX windowClass = {};
    inline WNDPROC originalWindowProcess = nullptr;

    // DirectX stuff
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline LPDIRECT3D9 d3d9 = nullptr;

    // Setup window class
    bool SetupWindowClass(const char* windowClassName) noexcept;

    // Destroy window class
    void DestroyWindowClass() noexcept;

    // Setup window
    bool SetupWindow(const char* windowName) noexcept;

    // Destroy window
    void DestroyWindow() noexcept;

    bool SetupDirectX() noexcept;

    void DestroyDirectX() noexcept;

    // Setup device
    void Setup();

    // Setup menu
    void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;

    // Destroy menu
    void DestroyMenu() noexcept;

    // Render menu
    void RenderMenu() noexcept;
}