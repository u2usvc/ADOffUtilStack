#ifdef FINDPID_GETNEXTPROCESS

#include <string>
#include <windows.h>
#include "../static/vars.hpp"
#include "../hash/hashes.hpp"
#include "../syscall/syscall.hpp"
#include "../static/debug.hpp"
#include <psapi.h>
#include <shlwapi.h>

EXTERN_C NTSTATUS sysNtGetNextProcess(
  HANDLE ProcessHandle,
  ACCESS_MASK DesiredAccess,
  ULONG HandleAttributes,
  ULONG Flags,
  PHANDLE NewProcessHandle
);

// EXTERN_C LPCSTR sysPathFindFileNameA(
//   LPCSTR pszPath
// );
//
// EXTERN_C DWORD sysGetProcessImageFileNameA(
//   HANDLE hProcess,
//   LPSTR  lpImageFileName,
//   DWORD  nSize
// );

DWORD findProcessId(std::string processName) {
  DEBUG_INFO("Starting NtGetNextProcess enumeration. Looking for: %s", processName.c_str());

  DWORD pid = 0;
  HANDLE hProcess = NULL;
  char procName[MAX_PATH];

  // loop through all processes
  while (NT_SUCCESS(call(
    hashNtDll,
    hashNtGetNextProcess,
    sysNtGetNextProcess,

    hProcess,
    MAXIMUM_ALLOWED,
    0,
    0,
    &hProcess
  ))) {

    memset(procName, 0, MAX_PATH);

    // store the name of the executable file for the process to procName
    DWORD nameLength = GetProcessImageFileNameA(
      hProcess,
      procName,
      MAX_PATH
    );
    // DWORD nameLength = call(
    //   hashNtDll,
    //   hashGetProcessImageFileNameA,
    //   sysGetProcessImageFileNameA,
    //
    //   hProcess,
    //   procName,
    //   MAX_PATH
    // );

    if (nameLength == 0) {
        continue;
    }

    // PathFindFileNameA extracts just the filename from a full path
    LPCSTR fileName = PathFindFileNameA(procName);
    // LPCSTR fileName = (LPCSTR)call(
    //   hashNtDll,
    //   hashPathFindFileNameA,
    //   sysPathFindFileNameA,
    //
    //   procName
    // );

    DEBUG_TRACE("Enumerated Handle: %p | Full Path: %s | File Name: %s", hProcess, procName, fileName);

    // Compare the extracted filename to the target processName
    if (lstrcmpiA(fileName, processName.c_str()) == 0) {
      pid = GetProcessId(hProcess);
      DEBUG_INFO("Match found! Process '%s' is running with PID: %lu", fileName, pid);

      break;
    }
  }

  if (pid == 0) {
      DEBUG_ERR("Failed to find process: %s", processName.c_str());
  }

  return pid;
}

#endif
