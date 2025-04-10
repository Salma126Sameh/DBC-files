#include "crc_calculator.h"
#include "main.h"

/*
uint32_t Calculate_CRC(const uint8_t *data, uint8_t length) {
    uint32_t crc = 0xFFFFFFFF; // Initial value
    hcrc.Instance->DR = crc; // Load initial CRC value

    for (uint8_t i = 0; i < length; i++) {
        hcrc.Instance->DR = data[i]; // Feed each byte into CRC calculator
    }

    return hcrc.Instance->DR ^ 0xFFFFFFFF; // Get final CRC value with XOR
}

*/




uint32_t Calculate_CRC(const uint8_t *data, uint8_t length) {
    uint32_t crc = 0;
    uint32_t buffer32[(256 + 3) / 4]; // Pre-allocate buffer for max data size
    uint32_t wordCount = (length + 3) / 4;
    uint8_t i, j;

    // Pack bytes into 32-bit words (little-endian)
    for (i = 0; i < wordCount; i++) {
        uint32_t word = 0;
        for (j = 0; j < 4 && (i * 4 + j) < length; j++) {
            word |= (uint32_t)data[i * 4 + j] << (j * 8);
        }
        buffer32[i] = word;
    }

    // Compute CRC using HAL function
    crc = HAL_CRC_Calculate(&hcrc, buffer32, wordCount);

    // Apply final XOR
    return crc ^ 0xFFFFFFFF;
}

