#include "vars.cpp"
#include <cpr/cpr.h>
#include <cstring>
#include <iostream>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <vector>

std::vector<unsigned char>
decrypt_instructions(const std::vector<unsigned char> eBytecode) {
  std::vector<unsigned char> plaintext;

  if (eBytecode.size() < 4 + 1 + 1 + 1)
    throw std::runtime_error("Encrypted data too small");

  if (std::string((char *)eBytecode.data(), 4) != "AESG")
    throw std::runtime_error("Bad magic");

  size_t pos = 4;
  unsigned char version = eBytecode[pos++];
  unsigned char saltLen = eBytecode[pos++];
  std::vector<unsigned char> salt(eBytecode.begin() + pos,
                                  eBytecode.begin() + pos + saltLen);
  pos += saltLen;

  unsigned char ivLen = eBytecode[pos++];
  std::vector<unsigned char> iv(eBytecode.begin() + pos,
                                eBytecode.begin() + pos + ivLen);
  pos += ivLen;

  std::vector<unsigned char> cipherAndTag(eBytecode.begin() + pos,
                                          eBytecode.end());
  if (cipherAndTag.size() < 16)
    throw std::runtime_error("Ciphertext too small");
  size_t tagLen = 16;
  std::vector<unsigned char> tag(cipherAndTag.end() - tagLen,
                                 cipherAndTag.end());
  std::vector<unsigned char> ciphertext(cipherAndTag.begin(),
                                        cipherAndTag.end() - tagLen);

  std::cout << "[+] Version: " << (int)version << "\n";
  std::cout << "[+] Salt length: " << (int)saltLen << " bytes\n";
  std::cout << "[+] IV length: " << (int)ivLen << " bytes\n";
  std::cout << "[+] Ciphertext length: " << ciphertext.size() << " bytes\n";
  std::cout << "[+] Tag length: " << tag.size() << " bytes\n";

  // Dump first few bytes for sanity
  std::cout << "[+] Salt: ";
  for (int i = 0; i < std::min((int)salt.size(), 8); i++)
    printf("%02X ", salt[i]);
  std::cout << "\n[+] IV: ";
  for (int i = 0; i < std::min((int)iv.size(), 8); i++)
    printf("%02X ", iv[i]);
  std::cout << "\n";

  // Derive key
  const int ITER = 100000;
  const int KEY_LEN = 32;
  std::vector<unsigned char> key(KEY_LEN);
  if (!PKCS5_PBKDF2_HMAC(KEY, strlen(KEY), salt.data(), (int)salt.size(), ITER,
                         EVP_sha256(), KEY_LEN, key.data())) {
    throw std::runtime_error("PBKDF2 failed");
  }

  std::cout << "[+] Derived key: ";
  for (int i = 0; i < 8; i++)
    printf("%02X ", key[i]);
  std::cout << "...\n";

  // AES-256-GCM decrypt
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx)
    throw std::runtime_error("EVP_CIPHER_CTX_new failed");

  int len = 0;
  int plaintext_len = 0;
  plaintext.resize(ciphertext.size());

  int ok = 1;
  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
    ok = 0;
  if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), NULL))
    ok = 0;
  if (1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key.data(), iv.data()))
    ok = 0;
  if (ciphertext.size() &&
      1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(),
                             (int)ciphertext.size()))
    ok = 0;
  plaintext_len += len;
  if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tagLen, tag.data()))
    ok = 0;
  if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len))
    ok = 0;
  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  if (!ok) {
    unsigned long e = ERR_get_error();
    while (e) {
      std::cerr << "OpenSSL error: " << ERR_error_string(e, NULL) << "\n";
      e = ERR_get_error();
    }
    throw std::runtime_error("Decryption failed");
  }

  plaintext.resize(plaintext_len);
  return plaintext;
}
