#include "decrypt.cpp"
#include "fetch.cpp"
#include "vars.cpp"

#include <iostream>
#include <vector>

int main() {
  try {
    std::vector<unsigned char> eBytecode = fetch_instructions();
    std::vector<unsigned char> decrypted = decrypt_instructions(eBytecode);
  } catch (const std::exception &e) {
    std::cerr << "[!] Exception: " << e.what() << "\n";
  }

  std::cout << "Press Enter to exit...";
  std::cin.get();

  return 0;
}
