#ifndef ENCODER_HEADER_INCLUDED
#define ENCODER_HEADER_INCLUDED

#include "bit_stream.h"
#include "sslms.h"
#include "block.h"
#include "code.h"
#include "tag.h"
#include "polynomial_predictor.h"
#include <stdbool.h>

typedef struct {
    FILE* output_file;                                  /* Output file. */
    bit_stream* output_bit_stream;                      /* Output bitstream. */
    uint32_t audio_data_size_position;
    uint32_t audio_data_begin_position;

    uint32_t sample_rate;                               /* Sample rate */
    uint8_t bits_per_sample;                            /* PCM quantization bits. */
    uint8_t num_channels;                               /* The number of channels. */
    uint32_t num_samples;                               /* The number of samples. */

    uint8_t filter_taps;                                /* The number of SSLMS filter taps. */
    uint16_t block_size;                                /* Block size. */
    bool use_mid_side_stereo;                           /* Mid-Side stereo flag. */
    uint32_t num_blocks;                                /* The number of blocks. */
    uint8_t reserved1;                                  /* Reserved. */
    uint8_t reserved2;                                  /* Reserved. */
    uint32_t audio_data_position_position;

    sslms** sslms_filters;                              /* SSLMS filters */
    polynomial_predictor** polynomial_predictors;       /* Polynomial predictors. */

    tag* tag;                                           /* The pointer to tag information. */
    code* coder;                                        /* The pointer to code. */
    block* current_block;                               /* The pointer to encoding block. */
    uint8_t current_sub_block_channel;                  /* Next sample offset. */
    uint16_t current_sub_block_offset;                  /* Next subblock offset. */
} encoder;

/*!
 * @brief                           Create new instance of encoder.
 * @param *file                     Output file
 * @param sample_rate               Sample rate
 * @param bits_per_sample           Quantization bits
 * @param num_channels              The number of channels.
 * @param num_samples               The number of samples.
 * @param block_size                Block size.
 * @param use_mid_side_stereo       Mid-Side stereo flag.
 * @param filter_taps               The taps of SSLMS filters.
 * @param *tag                      Tag information.
 */
encoder* encoder_create(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag);

/*!
 * @brief           Release encoder.
 * @param *encoder  Pointer to encoder.
 */
void encoder_free(encoder* encoder);

/*!
 * @brief           Encode sample.
 * @param *encoder  Pointer to encoder.
 * @param sample    Sample
 */
void encoder_write_sample(encoder* encoder, int32_t sample);

/*!
 * @brief           End encode.
 * @param *encoder  Pointer to encoder.
 */
void encoder_end_write(encoder* encoder);

#endif