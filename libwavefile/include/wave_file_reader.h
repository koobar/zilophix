#ifndef WAVE_FILE_READER_HEADER_INCLUDED
#define WAVE_FILE_READER_HEADER_INCLUDED

#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* wave_file;
    uint32_t stream_size;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
    uint32_t num_samples;
    uint32_t num_samples_read;
    uint32_t avr_bytes_per_sec;
    uint16_t block_size;
} wave_file_reader;

/*!
 * @brief           Create new instance of wave_file_reader.
 * @path            WAV file path.
 * @return          New pointer of wave_file_reader.
 */
wave_file_reader* wave_file_reader_create(const char* path);

/*!
 * @brief           Create new instance of wave_file_reader.
 * @path            File.
 * @return          New pointer of wave_file_reader.
 */
wave_file_reader* wave_file_reader_create_file(FILE* file);

/*!
 * @brief           Open WAV file.
 * @param *reader   Pointer of wave_file_reader.
 * @param *path     WAV file path.
 */
void wave_file_reader_open(wave_file_reader* reader, const char* path);

/*!
 * @brief           Close WAV file.
 * @param *reader   Pointer of wave_file_reader.
 */
void wave_file_reader_close(const wave_file_reader* reader);

/*!
 * @brief           Get sample rate.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Sample rate.
 */
uint32_t wave_file_reader_get_sample_rate(const wave_file_reader* reader);

/*!
 * @brief           Get bits per sample.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Bits per sample.
 */
uint16_t wave_file_reader_get_bits_per_sample(const wave_file_reader* reader);

/*!
 * @brief           Get number of channels.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Number of channels.
 */
uint16_t wave_file_reader_get_num_channels(const wave_file_reader* reader);

/*!
 * @brief           Get number of samples.
 * @param *reader   Pointer of wave_file_reader.
 * @return          Number of samples.
 */
uint32_t wave_file_reader_get_num_samples(const wave_file_reader* reader);

/*!
 * @brief           Read next sample.
 * @param *reader   Pointer of wave_file_reader
 * @return          Sample
 */
int32_t wave_file_reader_read_sample(wave_file_reader* reader);

#endif