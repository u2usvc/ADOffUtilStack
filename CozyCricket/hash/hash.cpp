#ifdef HASH_DJB2

#include <windows.h>

DWORD64 hash(const char *str) {
  DWORD64 dwHash = 0x6623662366236623;
  int c;

  while (c = *str++)
    dwHash = ((dwHash << 0x5) + dwHash) + c;

  return dwHash;
}

#endif
