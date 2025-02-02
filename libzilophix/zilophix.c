#include "./include/zilophix.h"
#include <string.h>

#define ID_AUDIO_DATA_OFFSET        0x52444441
#define ID_AUDIO_DATA_SIZE          0x455A4953

void zilophix_header_init(zilophix_header* header) {
    memset(header, 0, sizeof(zilophix_header));

    header->id_audio_data_offset = ID_AUDIO_DATA_OFFSET;
    header->id_audio_data_size = ID_AUDIO_DATA_SIZE;
}