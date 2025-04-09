/* Author: Luciano Scarpaci Copyright 2025 */
// host_messaging.c

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "host_messaging.h"


/** @brief Read len bytes from UART, acknowledging after every 256 bytes.
 * 
 *  @param buf Pointer to a buffer where the incoming bytes should be stored.
 *  @param len The number of bytes to be read.
 *  @param max_len The maximum number of bytes that can be read into the buffer. 
 *  @return 0 on success. A negative value on error.
*/
int read_bytes(void *buf, uint16_t len, uint16_t max_len) {
    if (buf == NULL) {
        return ERR_FAIL;
    }
    if (len > max_len) {
        return ERR_FAIL;
    }

    memset(buf, 0, len);
    int result;
    for (int i = 0; i < len; i++) {
        if (i % 256 == 0 && i != 0) { // Send an ACK after receiving 256 bytes
            write_ack();
        }
        result = uart_readbyte();
        if (result < 0) {  // if there was an error, return immediately
            return result;
        }
        ((uint8_t *)buf)[i] = (uint8_t)result;
    }

    return 0;
}

/** @brief Read a msg header from UART.
 * 
 *  @param hdr Pointer to a buffer where the incoming bytes should be stored.
*/
void read_header(msg_header_t *hdr) {
    if (hdr == NULL) {
        return; 
    }
    hdr->magic = uart_readbyte();
    // Any bytes until '%' will be read, but ignored.
    // Once we receive a '%', continue with processing the rest of the message.
    while (hdr->magic != MSG_MAGIC) {
        hdr->magic = uart_readbyte();
    }
    hdr->cmd = uart_readbyte();
    read_bytes(&hdr->len, sizeof(hdr->len), sizeof(hdr->len));
}

/** @brief Receive an ACK from UART.
 * 
 *  @return 0 on success. A negative value on error.
*/
uint8_t read_ack() {
    msg_header_t ack_buf = {0};

    read_header(&ack_buf);
    if (ack_buf.cmd == ACK_MSG) {
        return ERR_SUCCESS;
    } else {
        return ERR_FAIL;
    }
}

/** @brief Write len bytes to console
 * 
 *  @param buf Pointer to a buffer that stores the outgoing bytes.
 *  @param len The number of bytes to write.
 *  @param max_len The maximum number of bytes that can be written to the buffer.
 *  @param should_Ack True if the decoder should expect an ACK. This should be false for
 *                    debug and ACK messages. 
 *  @return 0 on success. A negative value on error.
*/
int write_bytes(const void *buf, uint16_t len, uint16_t max_len, bool should_ack) {
    if (buf == NULL) {
        return ERR_FAIL; 
    }
    if (len > max_len) {
        return ERR_FAIL;
    }
    for (int i = 0; i < len; i++) {
        if (i % 256 == 0 && i != 0) {  // Expect an ACK after sending every 256 bytes
            if (should_ack && read_ack() < 0) {
                return ERR_FAIL;
            }
        }
        uart_writebyte(((const uint8_t *)buf)[i]);
    }

    fflush(stdout);

    return ERR_SUCCESS;
}

/** @brief Write len bytes to UART in hex. 2 bytes will be printed for every byte.
 * 
 *  @param type Message type.
 *  @param buf Pointer to the bytes that will be printed.
 *  @param len The number of bytes to print.
 * 
 *  @return 0 on success. A negative value on error.
*/
int write_hex(msg_type_t type, const void *buf, size_t len) {
    if (buf == NULL) {
        return ERR_FAIL; 
    }
    msg_header_t hdr = {0};
    hdr.magic = MSG_MAGIC;
    hdr.cmd = type;
    hdr.len = len*2;

    if (write_bytes(&hdr, MSG_HEADER_SIZE, MSG_HEADER_SIZE, false) < 0) {
        return ERR_FAIL;
    }
    if (type != DEBUG_MSG && read_ack() < 0) {
        // If the header was not ack'd, don't send the message
        return ERR_FAIL;
    }

    for (size_t i = 0; i < len; i++) {
        if (i % (256 / 2) == 0 && i != 0) {
            if (type != DEBUG_MSG && read_ack() < 0) {
                // If the block was not ack'd, don't send the rest of the message
                return ERR_FAIL;
            }
        }
        printf("%02x", ((const uint8_t *)buf)[i]);
        fflush(stdout);
    }
    return ERR_SUCCESS;
}

/** @brief Send a message to the host, expecting an ack after every 256 bytes.
 * 
 *  @param type The type of message to send.
 *  @param buf Pointer to a buffer containing the outgoing packet.
 *  @param len The size of the outgoing packet in bytes.
 * 
 *  @return 0 on success. A negative value on failure.
*/
int write_packet(msg_type_t type, const void *buf, uint16_t len) {
    if (buf == NULL && len > 0) {
        return ERR_FAIL; 
    }
    msg_header_t hdr = {0};
    hdr.magic = MSG_MAGIC;
    hdr.cmd = type;
    hdr.len = len;

    int result = write_bytes(&hdr, MSG_HEADER_SIZE, MSG_HEADER_SIZE, false);
    if (type == ACK_MSG) {
        return result;
    }

    // If the header was not ack'd, don't send the message
    if (type != DEBUG_MSG && read_ack() < 0) {
        return ERR_FAIL;
    }
    // If there is data to write, write it
    if (len > 0) {
        result = write_bytes(buf, len, len, type != DEBUG_MSG);
        // If we still need to ACK the last block (write_bytes does not handle the final ACK)
        if (type != DEBUG_MSG && read_ack() < 0) {
            return ERR_FAIL;
        }
    }

    return ERR_SUCCESS;
}

/** @brief Reads a packet from console UART.
 * 
 *  @param cmd A pointer to the resulting opcode of the packet. Must not be null.
 *  @param buf A pointer to a buffer to store the incoming packet. Can be null.
 *  @param len A pointer to the resulting length of the packet. Can be null.
 * 
 *  @return 0 on success, a negative number on failure
*/
int read_packet(msg_type_t* cmd, void *buf, uint16_t *len) {
    if (cmd == NULL) {
        return ERR_FAIL; 
    }
    msg_header_t header = {0};
    read_header(&header);

    *cmd = header.cmd;

    if (len != NULL) {
        *len = header.len;
    }

    if (header.cmd != ACK_MSG) {
        write_ack();  // ACK the header
        if (header.len && buf != NULL && *len >= header.len) {
            if (read_bytes(buf, header.len, *len) < 0) {
                return ERR_FAIL;
            }
        }
        if (header.len) {
            if (write_ack() < 0) { // ACK the final block (not handled by read_bytes)
                return ERR_FAIL;
            }
        }
    }
    return ERR_SUCCESS;
}
