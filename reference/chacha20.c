#include <sodium.h>
#include <stdio.h>
#include <string.h>

int main() {
    // Initialize libsodium
    if (sodium_init() < 0) {
        // Panic! The library couldn't be initialized, it is not safe to use
        return 1;
    }

    // Define the message
    const char *message = "Crypto Example!";
    unsigned long long message_len = strlen(message);

    // Generate a random key and nonce
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(key, sizeof key);
    randombytes_buf(nonce, sizeof nonce);

    // Allocate memory for ciphertext
    unsigned char ciphertext[crypto_secretbox_MACBYTES + message_len];

    // Encrypt the message
    crypto_secretbox_easy(ciphertext, (const unsigned char *)message, message_len, nonce, key);

    // Allocate memory for decrypted message
    unsigned char decrypted_message[message_len];

    // Decrypt the message
    if (crypto_secretbox_open_easy(decrypted_message, ciphertext, sizeof ciphertext, nonce, key) != 0) {
        // Message forged!
        printf("Decryption failed!\n");
        return 1;
    }

    // Print the original, encrypted, and decrypted messages
    printf("Original Message: %s\n", message);
    printf("Encrypted Message: ");
    for (int i = 0; i < sizeof ciphertext; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");
    printf("Decrypted Message: %s\n", decrypted_message);

    return 0;
}
