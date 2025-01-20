#ifndef WAVE_FILE_WRITER_HEADER_INCLUDED
#define WAVE_FILE_WRITER_HEADER_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* wave_file;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
    uint32_t num_samples;
    bool flag_file_header_write;
    bool flag_fmt_chunk_write;
    bool flag_data_chunk_header_write;
} wave_file_writer;


/*!
 * @brief           Create new wave_file_writer.
 * @param *path     Output file path.
 * @return          Pointer to created wave_file_writer.
 */
wave_file_writer* wave_file_writer_create(const char* path);

/*!
 * @brief           Create new wave_file_writer.
 * @param *path     File.
 * @return          Pointer to created wave_file_writer.
 */
wave_file_writer* wave_file_writer_create_file(FILE* file);

/*!
 * @brief           Create or open WAV file.
 * @param *writer   Pointer to wave_file_writer.
 * @param *path     Path of WAV file.
 */
void wave_file_writer_open(wave_file_writer* writer, const char* path);

/*!
 * @brief           Close WAV file.
 * @param *writer   Pointer to wave_file_writer.
 */
void wave_file_writer_close(const wave_file_writer* writer);

/*!
 * @brief                   Set PCM format.
 * @param *writer           Pointer to wave_file_writer
 * @param sample_rate       Sample rate.
 * @param bits_per_sample   Quantization bits.
 * @param num_channels      Number of channels.
 */
void wave_file_writer_set_pcm_format(wave_file_writer* writer, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t num_channels);

/*!
 * @brief                   Set number of samples.
 * @param *writer           Pointer to wave_file_writer
 * @param num_samples       Number of samples.
 */
void wave_file_writer_set_num_samples(wave_file_writer* writer, uint32_t num_samples);

/*!
 * @brief                   Begin write samples.
 * @param *writer           Pointer to wave_file_writer.
 */
void wave_file_writer_begin_write(const wave_file_writer* writer);

/*!
 * @brief                   Write PCM sample to WAV file. You need call wave_file_writer_begin_write function before calling this function.
 * @param *writer           Pointer to wave_file_writer
 * @param sample            Sample
 */
void wave_file_writer_write_sample(const wave_file_writer* writer, int32_t sample);

/*!
 * @brief                   Stop write sample. This function must be called after all samples are written.
 * @param *writer           Pointer to wave_file_reader.
 */
void wave_file_writer_end_write(const wave_file_writer* writer);

#endif