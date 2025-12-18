#ifdef HASH_DJB2

#include <windows.h>

DWORD64 hash(const char *str) {
  DWORD64 dwHash = 0x7734773477347734;
  int c;

  while (c = *str++)
    dwHash = ((dwHash << 0x5) + dwHash) + c;

  return dwHash;
}

#endif
