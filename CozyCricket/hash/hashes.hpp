#include <windows.h>

// Common

constexpr DWORD64 hashNtDll                   = 3579829573855646769ULL;
constexpr DWORD64 hashNtOpenProcess           = 8162144977058099766ULL;
constexpr DWORD64 hashNtAllocateVirtualMemory = 9183676357489451690ULL;
constexpr DWORD64 hashNtWriteVirtualMemory    = 13414142115590362032ULL;
constexpr DWORD64 hashNtProtectVirtualMemory  = 5451768839971802726ULL;

#ifdef INJECT_EXTERNAL_EBAPC
  constexpr DWORD64 hashNtQueueApcThread        = 9077842422742839126ULL;
#endif

#ifdef INJECT_EXTERNAL_CREATE_MAP_SECTION
  constexpr DWORD64 hashNtCreateSection         = 14224581360565600814ULL;
  constexpr DWORD64 hashNtMapViewOfSection      = 8913094446144595464ULL;
  constexpr DWORD64 hashNtCreateThreadEx        = 8242583102987824718ULL;
#endif

#ifdef FINDPID_GETNEXTPROCESS
  constexpr DWORD64 hashNtGetNextProcess            = 4535614050878440003ULL;
  constexpr DWORD64 hashGetProcessImageFileNameA    = 17724974964480905095ULL;
  constexpr DWORD64 hashPathFindFileNameA           = 14516339943707692371ULL;
#endif
