#include "./include/wave_file_reader.h"
#include <stdbool.h>
#include <stdlib.h>

/*!
 * @brief       Reads an 8-bit integer from the specified file.
 * @param file  file
 * @return      value
 */
static uint8_t read_uint8(FILE* file) {
    uint8_t value;
    fread(&value, sizeof(uint8_t), 1, file);

    return value;
}

/*!
 * @brief       Reads an 16-bit integer from the specified file.
 * @param file  file
 * @return      value
 */
static uint16_t read_uint16(FILE* file) {
    uint16_t value;
    fread(&value, sizeof(uint16_t), 1, file);

    return value;
}

/*!
 * @brief       Reads an 32-bit integer from the specified file.
 * @param file  file
 * @return      value
 */
static uint32_t read_uint32(FILE* file) {
    uint32_t value;
    fread(&value, sizeof(uint32_t), 1, file);

    return value;
}

/*!
 * @brief       Reads an signed 16-bit integer from the specified file.
 * @param file  file
 * @return      value
 */
static int16_t read_int16(FILE* file) {
    int16_t value;
    fread(&value, sizeof(int16_t), 1, file);

    return value;
}

/*!
 * @brief       Reads an char from the specified file.
 * @param file  File
 * @return      Value
 */
static char read_char(FILE* file) {
    char value;
    fread(&value, sizeof(char), 1, file);

    return value;
}

/*!
 * @brief           Moves the read position of the file to the first occurrence of a byte sequence that matches the specified ASCII character.
 * @param *reader   Pointer of wave_file_reader.
 * @param *data     ASCII chars
 * @param n         Number of chars
 * @return          A value indicating whether a byte sequence corresponding to the specified ASCII character has occurred.
 */
static bool check_match_next_bytes(const wave_file_reader* reader, const char* data, uint32_t n) {
    register uint32_t i;
    register char read;

    for (i = 0; i < n; ++i) {
        read = read_char(reader->wave_file);

        if (read != data[i]) {
            return false;
        }
    }

    return true;
}

/*!
 * @brief                   Moves the read position of the file to the start of the chunk with the specified chunk name.
 * @param *reader           Pointer of wave_file_reader.
 * @param *chunk_name       Chunk name
 * @param n                 Number of chunk name chars.
 * @param find_from_begin   Flag indicating whether to seek chunks from the beginning of the file
 * @return                  A value indicating whether a chunk with the specified name was found.
 */
static bool go_to_chunk(const wave_file_reader* reader, const char* chunk_name, uint32_t n, bool find_from_begin) {
    if (find_from_begin) {
        fseek(reader->wave_file, 0, SEEK_SET);
    }

    for(;;) {
        if (check_match_next_bytes(reader, chunk_name, n)) {
            return true;
        }
    }

    return false;
}

/*!
 * @brief                   Read fmt chunk.
 */
static void read_fmt_chunk(wave_file_reader* reader) {
    uint16_t audio_format;

    if (go_to_chunk(reader, "fmt ", 4, true)) {
        read_uint32(reader->wave_file);                                     /* Skip chunk size (4 bytes)*/
        audio_format = read_uint16(reader->wave_file);

        if (audio_format == 0x0001 || audio_format == 0xFFFE) {
            reader->num_channels = read_uint16(reader->wave_file);          /* Number of channels. */
            reader->sample_rate = read_uint32(reader->wave_file);           /* Sample rate */
            reader->avr_bytes_per_sec = read_uint32(reader->wave_file);     /* Average bytes per second. */
            reader->block_size = read_uint16(reader->wave_file);            /* Block size */
            reader->bits_per_sample = read_uint16(reader->wave_file);       /* Quantization bits. */
        }
    }
}

/*!
 * @brief           Create new instance of wave_file_reader.
 * @path            WAV file path.
 * @return          New pointer of wave_file_reader.
 */
wave_file_reader* wave_file_reader_create(const char* path) {
    wave_file_reader* result = (wave_file_reader*)malloc(sizeof(wave_file_reader));
    
    if (result == NULL) {
        return NULL;
    }

    wave_file_reader_open(result, path);
    return result;
}

/*!
 * @brief           Open WAV file.
 * @param *reader   Pointer of wave_file_reader.
 * @param *path     WAV file path.
 */
void wave_file_reader_open(wave_file_reader* reader, const char* path) {
    uint32_t size;

    /* Open WAV file in read binary mode. */
    reader->wave_file = fopen(path, "rb");

    /* Read fmt chunk. */
    read_fmt_chunk(reader);

    /* Move to data chunk */
    if (go_to_chunk(reader, "data", 4, true)) {
        /* Calculate number of samples. */
        size = read_uint32(reader->wave_file);
        reader->num_samples = size / (reader->bits_per_sample / 8);
    }
    else {
        reader->num_samples = 0;
    }
}

/*!
 * @brief           Close WAV file.
 * @param *reader   Pointer of wave_file_reader.
 */
void wave_file_reader_close(const wave_file_reader* reader) {
    fclose(reader->wave_file);
}

/*!
 * @brief           Get sample rate.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Sample rate.
 */
uint32_t wave_file_reader_get_sample_rate(const wave_file_reader* reader) {
    return reader->sample_rate;
}

/*!
 * @brief           Get bits per sample.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Bits per sample.
 */
uint16_t wave_file_reader_get_bits_per_sample(const wave_file_reader* reader) {
    return reader->bits_per_sample;
}

/*!
 * @brief           Get number of channels.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Number of channels.
 */
uint16_t wave_file_reader_get_num_channels(const wave_file_reader* reader) {
    return reader->num_channels;
}

/*!
 * @brief           Get number of samples.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Number of samples.
 */
uint32_t wave_file_reader_get_num_samples(const wave_file_reader* reader) {
    return reader->num_samples;
}

/*!
 * @brief           Read next sample.
 * @param *reader   Pointer of wave_file_reader
 * @return          Sample
 */
int32_t wave_file_reader_read_sample(wave_file_reader* reader) {
    register int32_t result = 0;
    register uint8_t sb1, sb2;
    register int8_t sb3;

    switch (reader->bits_per_sample) {
    case 8:
        result = (int32_t)read_uint8(reader->wave_file) - 128;
        break;
    case 16:
        result = read_int16(reader->wave_file);
        break;
    case 24:
        sb1 = read_uint8(reader->wave_file);
        sb2 = read_uint8(reader->wave_file);
        sb3 = read_uint8(reader->wave_file);
        result = (sb3 << 16) | (sb2 << 8) | sb1;
        break;
    default:
        break;
    }

    reader->num_samples_read++;
    return result;
}