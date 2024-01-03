/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "../KKdLib/default.hpp"
#include "inject.hpp"
#include "print.hpp"
#include <Windows.h>

bool APIENTRY DllMain(HMODULE handle, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        printf_divagl("Current version - v0.1.3.0");
        printf_divagl("Attach");
        printf_divagl("Patching memory");
        inject_patches();
        break;
    }
    return true;
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void) {
    return L"DivaGL";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void) {
    return L"DivaGL Plugin by korenkonder";
}

extern "C" __declspec(dllexport) LPCWSTR GetBuildDate(void) {
    return L"v0.1.3.0";
}
