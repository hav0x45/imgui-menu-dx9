#include <stdexcept>
#include <intrin.h>
#include "hooks.h"
#include "../ext/minhook/minhook.h"
#include "../ext/imgui/imgui_impl_dx9.h"
#include "../ext/imgui/imgui_impl_win32.h"

void MyHooks::Setup() {

    if (MH_Initialize()) {
        throw std::runtime_error("Unable to initialize minhook");
    }

    if (MH_CreateHook(VirtualFunction(MyGui::device, 42), (LPVOID)&EndScene, reinterpret_cast<void**>(&EndSceneOriginal))) {
        throw std::runtime_error("Unable to hook EndScene()");
    }

    if (MH_CreateHook(VirtualFunction(MyGui::device, 16), (LPVOID)&Reset, reinterpret_cast<void**>(&ResetOriginal))) {
        throw std::runtime_error("Unable to hook Reset()");
    }

    if (MH_EnableHook(MH_ALL_HOOKS)) {
        throw std::runtime_error("Unable to enable hooks");
    }

    MyGui::DestroyDirectX();
}

void MyHooks::Destroy() noexcept {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

long __stdcall MyHooks::EndScene(IDirect3DDevice9* device) noexcept {
    static const auto returnAddress = _ReturnAddress();

    const auto result = EndSceneOriginal(device, device);

    // Stop endscene getting called twice
    if (_ReturnAddress() == returnAddress) {
        return result;
    }

    if (!MyGui::setup) {
        MyGui::SetupMenu(device);
    }

    if (MyGui::open) {
        MyGui::RenderMenu();
    }

    return result;
}

HRESULT __stdcall MyHooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    const auto result = ResetOriginal(device, device, params);
    ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}