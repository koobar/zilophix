#include "./include/zilophix.h"
#include "./include/macro.h"
#include <string.h>

#define ID_AUDIO_DATA_OFFSET        0x52444441
#define ID_AUDIO_DATA_SIZE          0x455A4953

/* Old shift factor table (for format version 1.21 or earlier only): { for 16-bits pcm, for 24-bits pcm }*/
static const int32_t shift_factor_table_old[2] = { 9, 8 };

/* New shift factor table (for format version 1.3 or above only) */
static const int32_t shift_factor_table_extended[32][2] = {
    /* Shift coefficients for small size SSLMS filter. */
    { 8, 8 }, { 8, 8 }, { 8, 8 }, { 8, 8 },
    { 8, 8 }, { 8, 8 }, { 8, 8 }, { 8, 8 },

    /* Shift coefficients for normal size SSLMS filter. */
    { 8, 8 }, { 8, 8 }, { 8, 8 }, { 8, 8 },
    { 8, 8 }, { 8, 8 }, { 8, 8 }, { 8, 8 },

    /* Shift coefficients for large size SSLMS filter. */
    { 9, 8 }, { 9, 8 }, { 9, 8 }, { 9, 8 },
    { 9, 8 }, { 9, 8 }, { 9, 8 }, { 9, 8 },

    /* Shift coefficients for extra size SSLMS filter. */
    { 9, 8 }, { 9, 8 }, { 9, 8 }, { 9, 8 },
    { 9, 8 }, { 9, 8 }, { 9, 8 }, { 9, 8 },
};

void zilophix_header_init(zilophix_header* header) {
    memset(header, 0, sizeof(zilophix_header));

    header->id_audio_data_offset = ID_AUDIO_DATA_OFFSET;
    header->id_audio_data_size = ID_AUDIO_DATA_SIZE;
}

const char* zilophix_get_version_name(uint8_t format_version) {
    switch (format_version) {
    case FORMAT_VERSION_1_0:
        return "1.0";
    case FORMAT_VERSION_1_1:
        return "1.1";
    case FORMAT_VERSION_1_2:
        return "1.2";
    case FORMAT_VERSION_1_3:
        return "1.3";
    }

    return "Unknown";
}

uint8_t zilophix_get_sslms_shift_coefficient(uint8_t sslms_taps, uint8_t format_version, uint8_t pcm_bits) {
    if (format_version <= FORMAT_VERSION_1_2) {
        return shift_factor_table_old[RSHIFT(pcm_bits, 3) - 2];
    }
    else if (format_version >= FORMAT_VERSION_1_3) {
        return shift_factor_table_extended[sslms_taps - 1][RSHIFT(pcm_bits, 3) - 2];
    }

    return 0;
}