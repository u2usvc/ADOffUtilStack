#ifdef RESOLVEAPI_DOS

#include <windows.h>
#include "../hash/hash.hpp"
#include <iostream>

// ==========================================
// DOS -> e_lfanew -> IMAGE_NT_HEADERS -> OptionalHeader[x] -> VirtualAddress => IMAGE_EXPORT_DIRECTORY -> AddressOfNames => `ApiSetQueryApiSetPresence`
// ==========================================
LPVOID resolve_api(HMODULE dllBase, DWORD64 apiHash) {
  // identical to
  // https://github.com/reveng007/DarkWidow/blob/main/src/indirect.cpp#L120

  // InLoadOrderModuleList.Flink.InLoadOrderLinks.DllBase: 0x7ffc35400000
  //
  // 0:003> dt IMAGE_DOS_HEADER 0x7ffc35400000␍
  // hizpdxvfmsautklyrwenbqocgj!IMAGE_DOS_HEADER␍
  //    +0x000 e_magic          : 0x5a4d␍
  //    +0x002 e_cblp           : 0x90␍
  //    +0x004 e_cp             : 3␍
  //    +0x006 e_crlc           : 0␍
  //    +0x008 e_cparhdr        : 4␍
  //    +0x00a e_minalloc       : 0␍
  //    +0x00c e_maxalloc       : 0xffff␍
  //    +0x00e e_ss             : 0␍
  //    +0x010 e_sp             : 0xb8␍
  //    +0x012 e_csum           : 0␍
  //    +0x014 e_ip             : 0␍
  //    +0x016 e_cs             : 0␍
  //    +0x018 e_lfarlc         : 0x40␍
  //    +0x01a e_ovno           : 0␍
  //    +0x01c e_res            : [4] 0␍
  //    +0x024 e_oemid          : 0␍
  //    +0x026 e_oeminfo        : 0␍
  //    +0x028 e_res2           : [10] 0␍
  //    +0x03c e_lfanew         : 0n216
  IMAGE_DOS_HEADER* DOS_HEADER = (IMAGE_DOS_HEADER*)dllBase;
  std::cout << "[+] DLL base (DOS header): " << dllBase << "\n";

  // 0:003> ? 0x7ffc35400000 + 0n216␍
  // Evaluate expression: 140721201873112 = 00007ffc`354000d8
  //
  // 0:003> dt nt!_IMAGE_NT_HEADERS64 00007ffc`354000d8␍
  // ntdll!_IMAGE_NT_HEADERS64␍
  //    +0x000 Signature        : 0x4550␍
  //    +0x004 FileHeader       : _IMAGE_FILE_HEADER␍
  //    +0x018 OptionalHeader   : _IMAGE_OPTIONAL_HEADER64
  IMAGE_NT_HEADERS* NT_HEADER = (IMAGE_NT_HEADERS*)((LPBYTE)dllBase + DOS_HEADER->e_lfanew);
  std::cout << "[+] NT Header: " << NT_HEADER << "\n";

  // 0:003> dt nt!_IMAGE_OPTIONAL_HEADER64 00007ffc`354000d8␍
  // ntdll!_IMAGE_OPTIONAL_HEADER64␍
  //    +0x000 Magic            : 0x4550␍
  //    +0x002 MajorLinkerVersion : 0 ''␍
  //    +0x003 MinorLinkerVersion : 0 ''␍
  //    +0x004 SizeOfCode       : 0x98664␍
  //    +0x008 SizeOfInitializedData : 0x103a4719␍
  //    +0x00c SizeOfUninitializedData : 0␍
  //    +0x010 AddressOfEntryPoint : 0␍
  //    +0x014 BaseOfCode       : 0x202200f0␍
  //    +0x018 ImageBase        : 0x00115600`0f0e020b␍
  //    +0x020 SectionAlignment : 0xd3400␍
  //    +0x024 FileAlignment    : 0␍
  //    +0x028 MajorOperatingSystemVersion : 0␍
  //    +0x02a MinorOperatingSystemVersion : 0␍
  //    +0x02c MajorImageVersion : 0x1000␍
  //    +0x02e MinorImageVersion : 0␍
  //    +0x030 MajorSubsystemVersion : 0␍
  //    +0x032 MinorSubsystemVersion : 0x3540␍
  //    +0x034 Win32VersionValue : 0x7ffc␍
  //    +0x038 SizeOfImage      : 0x1000␍
  //    +0x03c SizeOfHeaders    : 0x200␍
  //    +0x040 CheckSum         : 0xa␍
  //    +0x044 Subsystem        : 0xa␍
  //    +0x046 DllCharacteristics : 0␍
  //    +0x048 SizeOfStackReserve : 0xa␍
  //    +0x050 SizeOfStackCommit : 0x00000400`001f0000␍
  //    +0x058 SizeOfHeapReserve : 0x41600003`001f6eb1␍
  //    +0x060 SizeOfHeapCommit : 0x40000␍
  //    +0x068 LoaderFlags      : 0x1000␍
  //    +0x06c NumberOfRvaAndSizes : 0␍
  //    +0x070 DataDirectory    : [16] _IMAGE_DATA_DIRECTORY
  //
  // 0:003> dx -r1 (*((ntdll!_IMAGE_DATA_DIRECTORY (*)[16])0x7ffc35400160))␍
  // (*((ntdll!_IMAGE_DATA_DIRECTORY (*)[16])0x7ffc35400160))                 [Type: _IMAGE_DATA_DIRECTORY [16]]␍
  //     [0]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [1]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [2]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [3]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [4]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [5]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [6]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [7]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [8]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [9]              [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [10]             [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [11]             [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [12]             [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [13]             [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [14]             [Type: _IMAGE_DATA_DIRECTORY]␍
  //     [15]             [Type: _IMAGE_DATA_DIRECTORY]
  //
  // // let's get the [0]
  // 0:003> dt _IMAGE_DATA_DIRECTORY (00007ffc`354000d8 + 0x018 + 0x070)␍
  // hizpdxvfmsautklyrwenbqocgj!_IMAGE_DATA_DIRECTORY␍
  //    +0x000 VirtualAddress   : 0x14c470␍
  //    +0x004 Size             : 0x1276a
  PIMAGE_EXPORT_DIRECTORY EXdir = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)dllBase + NT_HEADER->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
  std::cout << "[+] NT header -> IMAGE_NT_HEADERS -> OptionalHeader[x] -> VirtualAddress => IMAGE_EXPORT_DIRECTORY: " << EXdir << "\n";

  // 0:003> ? 0x7ffc35400000 + 0x14c470␍
  // Evaluate expression: 140721203233904 = 00007ffc`3554c470
  //
  // // here's the structure of _IMAGE_EXPORT_DIRECTORY
  // // typedef struct _IMAGE_EXPORT_DIRECTORY ␍
  // // {␍
  // // 	DWORD   Characteristics;␍
  // // 	DWORD   TimeDateStamp;␍
  // // 	WORD    MajorVersion;␍
  // // 	WORD    MinorVersion;␍
  // // 	DWORD   Name;␍
  // // 	DWORD   Base;␍
  // // 	DWORD   NumberOfFunctions;␍
  // // 	DWORD   NumberOfNames;␍
  // // 	DWORD   AddressOfFunctions;     ␍
  // // 	DWORD   AddressOfNames;         ␍
  // // 	DWORD   AddressOfNameOrdinals;  ␍
  // // } IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
  //
  // // dump the memory contents of region where _IMAGE_EXPORT_DIRECTORY is supposed to reside
  // // we can see that AddressOfNames is `0014c498`
  // 0:003> dd 00007ffc`3554c470␍
  // 00007ffc`3554c470  00000000 103a4719 00000000 00152194␍
  // 00007ffc`3554c480  00000008 0000094d 0000094c 0014c498␍
  // 00007ffc`3554c490  0014e9cc 00150efc 0007cf40 0000c4d0␍
  // 00007ffc`3554c4a0  0000c600 0000c640 000dfa30 0006c550␍
  // 00007ffc`3554c4b0  000dfa60 000dfa80 0006edd0 0006ed90␍
  // 00007ffc`3554c4c0  00031df0 000850a0 0006ed30 00083bc0␍
  // 00007ffc`3554c4d0  00084f10 0006fc60 00085060 00085080␍
  // 00007ffc`3554c4e0  0006fc00 00075030 000d5730 0004c1c0
  //
  // // convert AddressOfNames RVA (0014c498) to address
  // 0:003> ? 0x7ffc35400000 + 0014e9cc␍
  // Evaluate expression: 140721203243468 = 00007ffc`3554e9cc␍
  //
  // // get a list of RVAs to API function name strings
  // 0:003> dd 00007ffc`3554e9cc␍
  // 00007ffc`3554e9cc  0015219e 001521a9 001521b3 001521bf␍
  // 00007ffc`3554e9dc  001521e8 00152206 00152232 00152259␍
  // 00007ffc`3554e9ec  0015226b 00152283 001522a4 001522d1␍
  // 00007ffc`3554e9fc  001522f0 0015230c 00152327 0015234e␍
  // 00007ffc`3554ea0c  00152368 00152385 001523ae 001523c8␍
  // 00007ffc`3554ea1c  001523e4 001523fd 00152417 0015242f␍
  // 00007ffc`3554ea2c  0015245b 00152473 00152485 00152499␍
  // 00007ffc`3554ea3c  001524b2 001524c7 001524d7 001524f2
  //
  // // read API function name strings (ANSI)
  // 0:003> da (0x7ffc35400000 + 0015219e)␍
  // 00007ffc`3555219e  "A_SHAFinal"␍
  // 0:003> da (0x7ffc35400000 + 001521a9)␍
  // 00007ffc`355521a9  "A_SHAInit"␍
  // 0:003> da (0x7ffc35400000 + 001523ae)␍
  // 00007ffc`355523ae  "ApiSetQueryApiSetPresence"␍
  // 0:003> da (0x7ffc35400000 + 001523c8)␍
  // 00007ffc`355523c8  "ApiSetQueryApiSetPresenceEx"
  PDWORD fAddr = (PDWORD)((LPBYTE)dllBase + EXdir->AddressOfFunctions);
	PDWORD fNames = (PDWORD)((LPBYTE)dllBase + EXdir->AddressOfNames);
	PWORD  fOrdinals = (PWORD)((LPBYTE)dllBase + EXdir->AddressOfNameOrdinals);
  std::cout << "[+] Got addresses of AddressOfFunctions, AddressOfNames, AddressOfNameOrdinals structures\n";
  std::cout << "[+] Traversing through AddressOfNames and comparing hashes\n";

  // for each byte in AddressOfFunctions
  // we conduct API hashing for the corresponding function name
  // and then return the appropriate function address referenced by ordinal
  // (because AddressOfFunctions is non-ordered)
  for (DWORD i = 0; i < EXdir->AddressOfFunctions; i++)
  {
    LPSTR pFuncName = (LPSTR)((LPBYTE)dllBase + fNames[i]);

    DWORD64 calculatedHash = hash(pFuncName);

    if (calculatedHash == apiHash)
    {
      LPVOID baseFuncAddr = (LPVOID)((LPBYTE)dllBase + fAddr[fOrdinals[i]]);
      std::cout << "[+] Found match on ordinal " << fOrdinals[i] << "\n";
      std::cout << "[+] Function name: " << pFuncName << "\n";
      std::cout << "[+] Function address: " << baseFuncAddr << "\n";
      return baseFuncAddr;
    }
  }
  return 0;
}

#endif
