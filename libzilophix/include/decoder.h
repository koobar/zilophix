#ifndef DECODER_HEADER_INCLUDED
#define DECODER_HEADER_INCLUDED

#include "bit_stream.h"
#include "sslms.h"
#include "block.h"
#include "code.h"
#include "polynomial_predictor.h"
#include "zilophix.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    FILE* file;                                     /* File */
    bit_stream* bit_stream;                         /* Bitstream */
    zilophix_header header;                         /* Header */

    sslms** sslms_filters;                          /* SSLMS filters. */
    polynomial_predictor** polynomial_predictors;   /* Polynomial predictors. */

    code* coder;                                    /* The pointer to code. */
    block* current_block;                           /* The pointer to decoding block. */
    uint8_t current_read_sub_block_channel;         /* The offset of subblock channel. */
    uint16_t current_read_sub_block_offset;         /* The offset of subblock sample reading offset. */

    uint32_t num_samples_read;                      /* The number of samples read. */
    bool is_seeking;                                /* Seeking flag. */
} decoder;

/*!
 * @brief                   Create new instance of decoder.
 * @param file*             Pointer to file.
 * @return                  Pointer to created instance.
 */
decoder* decoder_create(FILE* file);

/*!
 * @brief                   Release specified decoder.
 * @param decoder           Pointer to decoder.
 */
void decoder_free(decoder* decoder);

/*!
 * @brief                   Close decoding file.
 * @param decoder           Pointer to decoder.
 */
void decoder_close(decoder* decoder);

/*!
 * @brief                   Read next sample and return as PCM.
 * @param decoder           Pointer to decoder.
 * @return                  Sample
 */
int32_t decoder_read_sample(decoder* decoder);

/*!
 * @brief                   Seek to specified sample offset.
 * @param *decoder          Pointer to decoder.
 * @param sample_offset     Sample offset.
 */
void decoder_seek_sample_to(decoder* decoder, uint32_t sample_offset);

/*!
 * @brief                   Seek to specific time in milliseconds.
 * @param *decoder          Pointer to decoder.
 * @param sample_offset     Seek destination time in milliseconds.
 */
void decoder_seek_milliseconds_to(decoder* decoder, uint32_t ms);

/*!
 * @brief                   Get playback duration in milliseconds.
 * @param *decoder          Pointer to decoder.
 * @return                  Playback duration in milliseconds.
 */
uint32_t decoder_get_duration_ms(decoder* decoder);

#endif