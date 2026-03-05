#ifdef RESOLVEDLL_TIB

#include "../hash/hash.hpp"
#include "../types.hpp"
#include <iostream>
#include <windows.h>
#include "../helpers.cpp"
#include "../static/debug.hpp"

HMODULE resolve_dll(DWORD64 dllHash) {
  // identical to
  // https://github.com/reveng007/DarkWidow/blob/main/src/indirect.cpp#L196

  DEBUG_VERBOSE("Starting to resolve DLL with hash %llu", dllHash);

  PNT_TIB pTIB = NULL;
  PTEB pTEB = NULL;
  PPEB pPEB = NULL;

  // =================================================
  // TIB, TEB -> PEB -> Ldr -> InLoadOrderModuleList -> Flink ->
  // LDR_DATA_TABLE_ENTRY -> InLoadOrderLinks
  // =================================================
  //    +0x060 ProcessEnvironmentBlock : 0x000000b0`f035d000 _PEB
  //    +0x018 Ldr : 0x00007ffe`ffc853c0 _PEB_LDR_DATA
  //    +0x010 InLoadOrderModuleList : _LIST_ENTRY [ 0x000001d4`68542aa0 - 0x000001d4`68552890 ]␍
  //    -> Flink ->
  //    +0x000 InLoadOrderLinks : _LIST_ENTRY [ 0x000001d4`685428d0 - 0x00007ffe`ffc853d0 ]␍
  // =================================================
  // reads memory from GS segment (referenced by GS segment register) at
  // specified offset.
  pTIB = (PNT_TIB)__readgsqword(0x30);
  // 0:003> !teb␍
  // TEB at 000000b0f0364000␍
  //     ExceptionList:        0000000000000000␍
  //     StackBase:            000000b0f0c00000␍
  //     StackLimit:           000000b0f0bfc000␍
  //     SubSystemTib:         0000000000000000␍
  //     FiberData:            0000000000001e00␍
  //     ArbitraryUserPointer: 0000000000000000␍
  //     Self:                 000000b0f0364000␍
  //     EnvironmentPointer:   0000000000000000␍
  //     ClientId:             00000000000007d4 . 0000000000001c70␍
  //     RpcHandle:            0000000000000000␍
  //     Tls Storage:          0000000000000000␍
  //     PEB Address:          000000b0f035d000␍
  //     LastErrorValue:       0␍
  //     LastStatusValue:      0␍
  //     Count Owned Locks:    0␍
  //     HardErrorMode:        0

  // convert to PTEB
  pTEB = (PTEB)pTIB->Self;
  // {https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb}
  pPEB = (PPEB)pTEB->ProcessEnvironmentBlock;
  // 0:003> dt _TEB 000000b0f0364000 ProcessEnvironmentBlock␍
  // ntdll!_TEB␍
  //    +0x060 ProcessEnvironmentBlock : 0x000000b0`f035d000 _PEB

  if (pPEB == NULL) {
    DEBUG_ERR("Failed to get PEB");
    return NULL;
  }

  DEBUG_TRACE("TEB base: %p", pTIB);
  DEBUG_TRACE("PEB base: %p", pPEB);

  // {https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb}
  // typedef struct _PEB {
  //   ...
  //   PPEB_LDR_DATA                 Ldr;
  //   ...
  // } PEB, *PPEB;
  PPEB_LDR_DATA pPEB_LDR_DATA = (PPEB_LDR_DATA)(pPEB->Ldr);
  // 0:003> dt _PEB 0x000000b0`f035d000 Ldr␍
  // ntdll!_PEB␍
  //    +0x018 Ldr : 0x00007ffe`ffc853c0 _PEB_LDR_DATA

  // typedef struct _PEB_LDR_DATA {
  //   BYTE       Reserved1[8];
  //   PVOID      Reserved2[3];
  //   LIST_ENTRY InMemoryOrderModuleList;
  //   LIST_ENTRY InLoadOrderModuleList;
  // } PEB_LDR_DATA, *PPEB_LDR_DATA;
  PLIST_ENTRY ListHead, ListEntry;
  PLDR_DATA_TABLE_ENTRY LdrEntry;
  // 0:003> dt _PEB_LDR_DATA 0x00007ffe`ffc853c0␍
  // ntdll!_PEB_LDR_DATA␍
  //    +0x000 Length           : 0x58␍
  //    +0x004 Initialized      : 0x1 ''␍
  //    +0x008 SsHandle         : (null) ␍
  //    +0x010 InLoadOrderModuleList : _LIST_ENTRY [ 0x000001d4`68542aa0 - 0x000001d4`68552890 ]␍
  //    +0x020 InMemoryOrderModuleList : _LIST_ENTRY [ 0x000001d4`68542ab0 - 0x000001d4`685528a0 ]␍
  //    +0x030 InInitializationOrderModuleList : _LIST_ENTRY [ 0x000001d4`685428f0 - 0x000001d4`68549250 ]␍
  //    +0x040 EntryInProgress  : (null) ␍
  //    +0x048 ShutdownInProgress : 0 ''␍ +0x050 ShutdownThreadId : (null)

  // _LIST_ENTRY format is [ Flink - Blink ]

  ListHead = &pPEB->Ldr->InLoadOrderModuleList;
  // Flink - forward pointer
  ListEntry = ListHead->Flink;
  // 0:003> dt _LIST_ENTRY 0x000001d4`68542aa0␍
  // eyastwpfqjmvxlbhicuoznrkdg!_LIST_ENTRY␍
  //  [ 0x000001d4`685428d0 - 0x00007ffe`ffc853d0 ]␍
  //    +0x000 Flink            : 0x000001d4`685428d0 _LIST_ENTRY [ 0x000001d4`68543150 - 0x000001d4`68542aa0 ]␍
  //    +0x008 Blink            : 0x00007ffe`ffc853d0 _LIST_ENTRY [ 0x000001d4`68542aa0 - 0x000001d4`68552890 ]␍
  //
  // 0:003> dt _LDR_DATA_TABLE_ENTRY 0x000001d4`68542aa0␍
  // ntdll!_LDR_DATA_TABLE_ENTRY␍
  //    +0x000 InLoadOrderLinks : _LIST_ENTRY [ 0x000001d4`685428d0 - 0x00007ffe`ffc853d0 ]␍
  //    +0x010 InMemoryOrderLinks : _LIST_ENTRY [ 0x000001d4`685428e0 - 0x00007ffe`ffc853e0 ]␍
  //    +0x020 InInitializationOrderLinks : _LIST_ENTRY [ 0x00000000`00000000 - 0x00000000`00000000 ]␍
  //    +0x030 DllBase          : 0x00007ff6`5d390000 Void␍
  //    +0x038 EntryPoint       : 0x00007ff6`5d3913f0 Void␍
  //    +0x040 SizeOfImage      : 0xebe000␍
  //    +0x048 FullDllName      : _UNICODE_STRING "C:\Users\alex\Desktop\eyastwpfqjmvxlbhicuoznrkdg.exe"␍
  //    +0x058 BaseDllName      : _UNICODE_STRING "eyastwpfqjmvxlbhicuoznrkdg.exe"␍
  //    +0x068 FlagGroup        : [4]  "???"␍
  //    +0x068 Flags            : 0x22cc␍
  //    +0x06c ObsoleteLoadCount : 0xffff␍
  //    +0x06e TlsIndex         : 0xffff␍
  //    +0x070 HashLinks        : _LIST_ENTRY [ 0x000001d4`68546100 - 0x00007ffe`ffc85070 ]␍
  //    +0x080 TimeDateStamp    : 0x6943e4a1␍
  //    +0x098 DdagNode         : 0x000001d4`68542bf0 _LDR_DDAG_NODE␍
  //    +0x0a0 NodeModuleLink   : _LIST_ENTRY [ 0x000001d4`68542bf0 - 0x000001d4`68542bf0 ]␍
  //    +0x0c0 SwitchBackContext : 0x00007ffe`ffc38274 Void␍
  //    +0x0c8 BaseAddressIndexNode : _RTL_BALANCED_NODE␍
  //    +0x0e0 MappingInfoIndexNode : _RTL_BALANCED_NODE␍
  //    +0x0f8 OriginalBase     : 0x00007ff6`5d390000␍
  //    +0x100 LoadTime         : _LARGE_INTEGER 0x01dc709b`dfe466e2␍
  //    +0x108 BaseNameHashValue : 0x80e41c03␍

  DEBUG_TRACE("PEB.Ldr.InLoadOrderModuleList: %p", ListHead);
  DEBUG_TRACE("InLoadOrderModuleList.Flink: %p", ListEntry);
  // ==============================================
  // retrieved InLoadOrderModuleList successfully
  // ==============================================

  // an iterator through each Flink in InLoadOrderModuleList
  while (ListHead != ListEntry) {
    // check if ListEntry of LDR_DATA_TABLE_ENTRY contains field
    // InLoadOrderLinks and return this field's address if it does.
    // InLoadOrderLinks is defined manually in types.hpp.
    LdrEntry =
        CONTAINING_RECORD(ListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
    //  0:003> dt _LDR_DATA_TABLE_ENTRY 0x000001d4`685428d0 InLoadOrderLinks␍
    //ntdll!_LDR_DATA_TABLE_ENTRY␍
    //   +0x000 InLoadOrderLinks : _LIST_ENTRY [ 0x000001d4`68543150 - 0x000001d4`68542aa0 ]
    DEBUG_TRACE("InLoadOrderModuleList.Flink.InLoadOrderLinks: %p", LdrEntry);

    // Loading loaded DllBase Address from:
    // Loader Data Table Entry (LDR_DATA_TABLE_ENTRY struture) -> present in
    // ntapi.h file
    UNICODE_STRING BaseDllName = (LdrEntry->FullDllName);
    DEBUG_TRACE("InLoadOrderModuleList.Flink.InLoadOrderLinks.FullDllName: %p", PWSTR_to_Char(BaseDllName.Buffer));
    HMODULE DllBase = (HMODULE)(LdrEntry->DllBase);
    DEBUG_TRACE("InLoadOrderModuleList.Flink.InLoadOrderLinks.DllBase: %p", DllBase);

    // ================================== Checking Passed API hash
    // DWORD64 retrievedhash = create_hash(BaseDllName.Buffer);
    const char *Dllname = PWSTR_to_Char(BaseDllName.Buffer);
    DWORD64 retrievedhash = hash(Dllname);
    DEBUG_TRACE("Retrieved hash: %llu", retrievedhash);
    DEBUG_TRACE("Dll hash: %llu", dllHash);

    if (retrievedhash == dllHash) {
      DEBUG_VERBOSE("Dll resolved");
      // printf("BaseDllName: %ws (addr: %p)\n\n", BaseDllName.Buffer, DllBase);
      return DllBase;
    }
    // ================================== End: Checking Passed API hash

    /* Advance to the next module */
    ListEntry = ListEntry->Flink;
  }
  return 0;
}

#endif
