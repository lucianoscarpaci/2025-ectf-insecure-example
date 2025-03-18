// cryptography.h

#include <stdint.h>
#include <stdio.h>
#include "host_messaging.h"
#include "secrets.h"
#include "wolfssl/wolfcrypt/aes.h"

int decrypt_frame(uint8_t * frame_data, uint16_t frame_size);

