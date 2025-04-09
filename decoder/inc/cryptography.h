/* Author: Luciano Scarpaci Copyright 2025 */
// cryptography.h

#include <stdint.h>
#include <stdio.h>
#include "host_messaging.h"
#include "secrets.h"
#include "wolfssl/wolfcrypt/aes.h"
#include "wolfssl/wolfcrypt/hash.h"

int decrypt_frame(uint8_t * frame_data, uint16_t frame_size);

int hash(void * frame_data, size_t len, uint8_t * hash_out);
