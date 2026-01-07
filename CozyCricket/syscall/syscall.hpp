#include <windows.h>

WORD GetSyscallNum(LPVOID ntapiaddr);
DWORD64 GetSyscallAddr(LPVOID ntapiaddr);

#ifdef SYSCALL_INDIRECT

#include "../resolve/resolve.hpp"
#include "../types.hpp"

EXTERN_C VOID PrepSyscallNum(WORD SSN);
EXTERN_C VOID PrepSyscallAddr(INT_PTR syscallAddr);

template<typename Fn, typename... Args>
auto call(DWORD64 dllHash, DWORD64 apiHash, Fn func, Args... args) -> decltype(func(args...)) {
  // ntdll
  HMODULE dllBase = resolve_dll(dllHash);
  // NtOpenFile
  LPVOID pApi = resolve_api(dllBase, apiHash);

  WORD syscallNum = GetSyscallNum(pApi);
  DWORD64 syscallAddr = GetSyscallAddr(pApi);

  PrepSyscallNum(syscallNum);
  PrepSyscallAddr(syscallAddr);

  return func(args...);
}

#endif
