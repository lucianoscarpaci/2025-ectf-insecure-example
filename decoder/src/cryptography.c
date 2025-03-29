/* Author: Luciano Scarpaci Copyright 2025 */
// cryptography.c
#include "cryptography.h"

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
