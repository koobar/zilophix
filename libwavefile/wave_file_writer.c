#include "./include/wave_file_writer.h"
#include <stdlib.h>

/*!
 * @brief       Writes the specified ASCII character to the file.
 * @param file  File
 * @param value Value to write
 */
static void write_char(FILE* file, const char value) {
    fwrite(&value, sizeof(char), 1, file);
}

/*!
 * @brief       Writes a 16-bit integer to a file.
 * @param file  File
 * @param value Value to write
 */
static void write_uint16(FILE* file, const uint16_t value) {
    fwrite(&value, sizeof(uint16_t), 1, file);
}

/*!
 * @brief       Writes a 32-bit integer to a file.
 * @param file  File
 * @param value Value to write
 */
static void write_uint32(FILE* file, const uint32_t value) {
    fwrite(&value, sizeof(uint32_t), 1, file);
}

/*!
 * @brief       Writes a signed 16-bit integer to a file.
 * @param file  File
 * @param value Value to write
 */
static void write_int16(FILE* file, const int16_t value) {
    fwrite(&value, sizeof(int16_t), 1, file);
}

/*!
 * @brief           Calculate chunk size.
 * @param *writer   Pointer of wave_file_writer.
 * @return          Chunk size.
 */
static uint32_t compute_chunk_size(const wave_file_writer* writer) {
    uint32_t size_of_fmt_chunk = 24;
    uint32_t bytes_per_sample = writer->bits_per_sample / 8;
    uint32_t size_of_data_chunk = 4 + writer->num_samples * bytes_per_sample;
    uint32_t chunk_size = size_of_data_chunk + size_of_fmt_chunk;

    return chunk_size;
}

/*!
 * @brief           Write wave file header.
 * @param *writer   Pointer of wave_file_writer
 */
static void write_wave_header(const wave_file_writer* writer) {
    /* Write RIFF magic numbers. */
    write_char(writer->wave_file, 0x52);
    write_char(writer->wave_file, 0x49);
    write_char(writer->wave_file, 0x46);
    write_char(writer->wave_file, 0x46);

    /* Write chunk size. */
    write_uint32(writer->wave_file, compute_chunk_size(writer));

    /* Write 'WAVE' as ASCII string. */
    write_char(writer->wave_file, 0x57);
    write_char(writer->wave_file, 0x41);
    write_char(writer->wave_file, 0x56);
    write_char(writer->wave_file, 0x45);
}

/*!
 * @brief           Write fmt chunk
 * @param *writer   Pointer of wave_file_writer
 */
static void write_fmt_chunk(const wave_file_writer* writer) {
    uint32_t avr_bytes_per_sec;
    uint16_t block_size;

    /* Write fmt chunk header. */
    write_char(writer->wave_file, 0x66);
    write_char(writer->wave_file, 0x6d);
    write_char(writer->wave_file, 0x74);
    write_char(writer->wave_file, 0x20);

    /* Write fmt chunk size. */
    write_uint32(writer->wave_file, 16);

    /* Write audio format code. */
    write_uint16(writer->wave_file, 0x0001);

    /* Write number of channels. */
    write_uint16(writer->wave_file, writer->num_channels);

    /* Write sample rate. */
    write_uint32(writer->wave_file, writer->sample_rate);

    /* Write average bytes per second. */
    avr_bytes_per_sec = writer->sample_rate * (writer->bits_per_sample / 8) * writer->num_channels;
    write_uint32(writer->wave_file, avr_bytes_per_sec);

    /* Write block size. */
    block_size = writer->num_channels * writer->bits_per_sample / 8;
    write_uint16(writer->wave_file, block_size);

    /* Write PCM quantization bits per sample. */
    write_uint16(writer->wave_file, writer->bits_per_sample);
}

/*!
 * @brief           Write data chunk.
 * @param *writer   Pointer of wave_file_writer.
 */
static void write_data_chunk_header(const wave_file_writer* writer) {
    uint32_t bytes_per_sample, data_chunk_size;

    /* Write 'data' as ASCII string. */
    write_char(writer->wave_file, 0x64);
    write_char(writer->wave_file, 0x61);
    write_char(writer->wave_file, 0x74);
    write_char(writer->wave_file, 0x61);

    /* Write chunk size. */
    bytes_per_sample = writer->bits_per_sample / 8;
    data_chunk_size = writer->num_samples * bytes_per_sample;
    write_uint32(writer->wave_file, data_chunk_size);
}

/*!
 * @brief           Create new instance of wave_file_writer.
 * @param *path     Output file path.
 * @return          Pointer of wave_file_writer.
 */
wave_file_writer* wave_file_writer_create(const char* path) {
    wave_file_writer* result = (wave_file_writer*)malloc(sizeof(wave_file_writer));

    if (result == NULL) {
        return NULL;
    }

    wave_file_writer_open(result, path);
    return result;
}

/*!
 * @brief           Create or open WAV file.
 * @param *writer   Pointer of wave_file_writer.
 * @param *path     Path of WAV file.
 */
void wave_file_writer_open(wave_file_writer* writer, const char* path) {
    writer->wave_file = fopen(path, "wb");
    writer->sample_rate = 0;
    writer->bits_per_sample = 0;
    writer->num_channels = 0;
    writer->num_samples = 0;
    writer->flag_file_header_write = false;
    writer->flag_fmt_chunk_write = false;
    writer->flag_data_chunk_header_write = false;
}

/*!
 * @brief           Close WAV file.
 * @param *writer   Pointer of wave_file_writer.
 */
void wave_file_writer_close(const wave_file_writer* writer) {
    fclose(writer->wave_file);
    free(writer->wave_file);
}

/*!
 * @brief                   Set PCM format.
 * @param *writer           Pointer of wave_file_writer
 * @param sample_rate       Sample rate.
 * @param bits_per_sample   Quantization bits.
 * @param num_channels      Number of channels.
 */
void wave_file_writer_set_pcm_format(wave_file_writer* writer, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t num_channels) {
    writer->sample_rate = sample_rate;
    writer->bits_per_sample = bits_per_sample;
    writer->num_channels = num_channels;
}

/*!
 * @brief                   Set number of samples.
 * @param *writer           Pointer of wave_file_writer
 * @param num_samples       Number of samples.
 */
void wave_file_writer_set_num_samples(wave_file_writer* writer, uint32_t num_samples) {
    writer->num_samples = num_samples;
}

/*!
 * @brief                   Begin write samples.
 * @param *writer           Pointer of wave_file_writer.
 */
void wave_file_writer_begin_write(const wave_file_writer* writer) {
    write_wave_header(writer);
    write_fmt_chunk(writer);
    write_data_chunk_header(writer);
}

/*!
 * @brief                   Write PCM sample to WAV file. You need call wave_file_writer_begin_write function before calling this function.
 * @param *writer           Pointer of wave_file_writer
 * @param sample            Sample
 */
void wave_file_writer_write_sample(const wave_file_writer* writer, int32_t sample) {
    switch (writer->bits_per_sample)
    {
    case 16:
        write_int16(writer->wave_file, (int16_t)sample);
        break;
    case 24:
        write_char(writer->wave_file, (char)(sample >> 0) & 0xFF);
        write_char(writer->wave_file, (char)(sample >> 8) & 0xFF);
        write_char(writer->wave_file, (char)(sample >> 16) & 0xFF);
        break;
    }
}

/*!
 * @brief                   Stop write sample. This function must be called after all samples are written.
 * @param *writer           Pointer of wave_file_reader.
 */
void wave_file_writer_end_write(const wave_file_writer* writer) {
    fflush(writer->wave_file);
}