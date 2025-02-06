#include <Windows.h>
#include "gui.h"

#include "../ext/imgui/imgui_impl_dx9.h"
#include "../ext/imgui/imgui_impl_win32.h"

#include <stdexcept>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParam,
    LPARAM longParam
);

// Window process
LRESULT WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParam,
    LPARAM longParam
);

// Setup window class
bool MyGui::SetupWindowClass(const char* windowClassName) noexcept {

    // Populate window class
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = DefWindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = NULL;
    windowClass.hIconSm = NULL;

    // Register class
    if (!RegisterClassEx(&windowClass)) {
        return false;
    }

    return true;
}

// Destroy window class
void MyGui::DestroyWindowClass() noexcept {

    UnregisterClass(
        windowClass.lpszClassName,
        windowClass.hInstance
    );
}

// Setup window
bool MyGui::SetupWindow(const char* windowName) noexcept {

    // Create temp window
    window = CreateWindowA((const char*)windowClass.lpszClassName, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 0, 0, windowClass.hInstance, 0);

    if (!window) {
        return false;
    }

    return true;
}

// Destroy window
void MyGui::DestroyWindow() noexcept {

    if (window) {
        DestroyWindow(window);
    }
}

bool MyGui::SetupDirectX() noexcept {

    const auto handle = GetModuleHandleA("d3d9.dll");

    if (!handle) {
        return false;
    }

    using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

    const auto create = reinterpret_cast<CreateFn>(GetProcAddress(handle, "Direct3DCreate9"));

    if (!create) {
        return false;
    }

    d3d9 = create(D3D_SDK_VERSION);

    if (!d3d9) {
        return false;
    }

    D3DPRESENT_PARAMETERS params = {};
    params.BackBufferWidth = 0;
    params.BackBufferHeight = 0;
    params.BackBufferFormat = D3DFMT_UNKNOWN;
    params.BackBufferCount = 0;
    params.MultiSampleType = D3DMULTISAMPLE_NONE;
    params.MultiSampleQuality = 0;
    params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.hDeviceWindow = window;
    params.Windowed = 1;
    params.EnableAutoDepthStencil = 0;
    params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    params.Flags = 0;
    params.FullScreen_RefreshRateInHz = 0;
    params.PresentationInterval = 0;

    if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &params, &device) < 0) {
        return false;
    }

    return true;
}


void MyGui::DestroyDirectX() noexcept {

    if (device) {
        device->Release();
        device = NULL;
    }

    if (d3d9) {
        d3d9->Release();
        d3d9 = NULL;
    }
}


// Setup device
void MyGui::Setup() {

    if (!SetupWindowClass("Window Class!")) {
        throw std::runtime_error("Failed to create window class.");
    }

    if (!SetupWindow("Game Window!")) {
        throw std::runtime_error("Failed to create window!");
    }

    if (!SetupDirectX()) {
        throw std::runtime_error("Failed to create device!");
    }

    DestroyWindow();
    DestroyWindowClass();
}

// Setup menu
void MyGui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept {

    auto params = D3DDEVICE_CREATION_PARAMETERS{};
    device->GetCreationParameters(&params);

    window = params.hFocusWindow;

    originalWindowProcess = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);

    setup = true;
}

// Destroy menu
void MyGui::DestroyMenu() noexcept {

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // Restore wndproc
    SetWindowLongPtr(
        window,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(originalWindowProcess)
    );

    DestroyDirectX();
}

// Render menu
void MyGui::RenderMenu() noexcept {

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("My Menu", &open);
    ImGui::End();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

LRESULT WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParam,
    LPARAM longParam
) {

    // Toggle menu
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        MyGui::open = !MyGui::open;
    }

    // Pass messages to ImGui
    if (MyGui::open && ImGui_ImplWin32_WndProcHandler(
        window,
        message,
        wideParam,
        longParam
    )) return 1L;

    return CallWindowProc(
        MyGui::originalWindowProcess,
        window,
        message,
        wideParam,
        longParam
    );
}