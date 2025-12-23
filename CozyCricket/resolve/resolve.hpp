#pragma once

#include <windows.h>

HMODULE resolve_dll(DWORD64 dllHash);

LPVOID resolve_api(HMODULE dllBase, DWORD64 apiHash);
