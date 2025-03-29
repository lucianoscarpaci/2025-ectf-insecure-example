/* Author: Luciano Scarpaci Copyright 2025 */
// cryptography.c
#include "cryptography.h"
#include "aes.h"
#include "board.h"
#include "dma.h"
#include "mxc_device.h"
volatile int dma_flag = 0;

void DMA0_IRQHandler(void) {
  MXC_DMA_Handler();
  dma_flag++;
}

int decrypt_frame(uint8_t *frame_data, uint16_t frame_size) {
  Aes decrypt;
  mxc_aes_req_t req;
  int result;
  const uint8_t key[32] = SK;
  const uint8_t iv[16] = IV;

  MXC_DMA_ReleaseChannel(0);
  NVIC_EnableIRQ(DMA0_IRQn);

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

  req.length = frame_size;
  req.inputData = frame_data;
  req.resultData = frame_data;
  req.keySize = MXC_AES_256BITS;
  req.encryption = MXC_AES_DECRYPT_INT_KEY;

  result = MXC_AES_DecryptAsync(&req);

  DMA0_IRQHandler();
  while (dma_flag == 0) {
  }

  dma_flag = 0;

  // Perform decryption using wolfSSL
  result = wc_AesCbcDecrypt(&decrypt, frame_data, frame_data, frame_size);
  if (result) {
    result = print_debug("wc_AesCbcDecrypt Failure");
    return result;
  }
  MXC_AES_Shutdown();
  wc_AesFree(&decrypt);
  return 0;
}
