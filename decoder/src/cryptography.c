// cryptography.c
#include "cryptography.h"

int decrypt_frame(uint8_t * frame_data, uint16_t frame_size) {
	Aes decrypt;
	int result;
	const uint8_t key[16] = SK;
	const uint8_t iv[16] = IV;
	// TODO: possibly define a heap hint location
	// TODO: possibly enable use of hardware crypt if aviable
	result = wc_AesInit(&decrypt, NULL, INVALID_DEVID);
	if (result) {
		print_debug("wc_AesInit Failure");
		return result;
	}

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
