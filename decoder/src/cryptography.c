/* Author: Luciano Scarpaci Copyright 2025 */
// cryptography.c
#include "cryptography.h"
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha512.h>

int decrypt_frame(uint8_t *frame_data, uint16_t frame_size) {
  Aes decrypt;
  int result;
  const uint8_t key[32] = SK;
  const uint8_t iv[16] = IV;

  result = wc_AesInit(&decrypt, NULL, INVALID_DEVID);
  if (result) {
    printf("wc_AesInit Failure\n");
    return result;
  }

  printf("Setting AES key\n");
  result = wc_AesSetKey(&decrypt, key, sizeof(key), iv, AES_DECRYPTION);
  if (result) {
    print_debug("wc_AesSetKey Failure");
    return result;
  }

  result = wc_AesCbcDecrypt(&decrypt, frame_data, frame_data, frame_size);
  if (result) {
    result = print_debug("wc_AesCbcDecrypt Failure");
    return result;
  }
  wc_AesFree(&decrypt);
  return 0;
}

int hash(void *frame_data, size_t len, uint8_t *hash_out) {
  return wc_Sha512Hash((uint8_t *)frame_data, len, hash_out);
}