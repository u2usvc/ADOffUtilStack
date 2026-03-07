#ifdef PATCH_ETW_USER

#include <minwindef.h>
#include <windows.h>

int patch() {
  DWORD dwOld = 0;
  FARPROC ptrNtTraceEvent =
      GetProcAddress(LoadLibrary("ntdll.dll"), "NtTraceEvent");

  LPVOID pNtTraceEvent = reinterpret_cast<LPVOID>(ptrNtTraceEvent);

  VirtualProtect(pNtTraceEvent, 1, PAGE_EXECUTE_READWRITE, &dwOld);
  memcpy(pNtTraceEvent, "\xc3", 1);
  VirtualProtect(pNtTraceEvent, 1, dwOld, &dwOld);
  return 0;
}

#elifdef PATCH_ETW_NONE

int patch() {
  return 0;
}

#endif
