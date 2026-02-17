#include <windows.h>

WORD GetSyscallNum(LPVOID ntapiaddr);
DWORD64 GetSyscallAddr(LPVOID ntapiaddr);

#ifdef SYSCALL_INDIRECT

#include "../resolve/resolve.hpp"
#include "../types.hpp"

EXTERN_C VOID PrepSyscallNum(WORD SSN);
EXTERN_C VOID PrepSyscallAddr(INT_PTR syscallAddr);

EXTERN_C NTSTATUS sysNtAllocateVirtualMemory(
	HANDLE    ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR ZeroBits,
	PSIZE_T   RegionSize,
	ULONG     AllocationType,
	ULONG     Protect
);

EXTERN_C NTSTATUS sysNtProtectVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN OUT PSIZE_T RegionSize,
	IN ULONG NewProtect,
	OUT PULONG OldProtect
);


EXTERN_C NTSTATUS sysNtWriteVirtualMemory(
	IN HANDLE               ProcessHandle,
	IN PVOID                BaseAddress,
	IN PVOID                Buffer,
	IN SIZE_T                NumberOfBytesToWrite,
	OUT PULONG              NumberOfBytesWritten
);

EXTERN_C NTSTATUS sysNtOpenProcess(
	PHANDLE ProcessHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID ClientId
);

typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		VOID* Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef VOID(NTAPI* PIO_APC_ROUTINE)(
	IN PVOID            ApcContext,
	IN PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG            Reserved
	);

EXTERN_C NTSTATUS sysNtQueueApcThread(

	HANDLE ThreadHandle,
	PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcRoutineContext OPTIONAL,
	PIO_STATUS_BLOCK ApcStatusBlock OPTIONAL,
	ULONG ApcReserved OPTIONAL
);

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
