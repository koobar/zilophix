#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/zilophix.h"
#include "./include/encoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include <stdlib.h>
#include <string.h>

#define DUMMY_VALUE         0xFFFFFFFF  

/*!
 * @brief           Write magic numbers
 * @param *encoder  Pointer to encoder.
*/
static inline void write_magic_number(encoder* encoder){
    write_uint8(encoder->output_file, 0x5A);
    write_uint8(encoder->output_file, 0x70);
    write_uint8(encoder->output_file, 0x58);
}

/*!
 * @brief           Write header.
 * @param *encoder  Pointer to encoder.
 */
static inline void write_header(encoder* encoder) {
    write_magic_number(encoder);
    write_mem(encoder->output_file, &encoder->header, sizeof(zilophix_header));
}

/*!
 * @brief           Write audio data offset and audio data size.
 * @param *encoder  Pointer to encoder.
*/
static inline void flush_audio_data(encoder* encoder){
    uint32_t current_len = (uint32_t)ftell(encoder->output_file);
    uint32_t data_size = current_len - (encoder->audio_data_begin_position);

    /* Move to address of audio data begin offset and write audio data offset */
    fseek(encoder->output_file, HEADER_OFFSET_OF_AUDIO_DATA_OFFSET, SEEK_SET);
    write_uint32(encoder->output_file, encoder->audio_data_begin_position);

    /* Move to address of audio data size and write audio data size. */
    fseek(encoder->output_file, HEADER_OFFSET_OF_AUDIO_DATA_SIZE, SEEK_SET);
    write_uint32(encoder->output_file, data_size);

    /* Move to default position. */
    fseek(encoder->output_file, current_len, SEEK_SET);
}

/*!
 * @brief           Encode subblock
 * @param *poly     The pointer to polynomial predictor.
 * @param *sslms    The pointer to SSLMS filter.
 * @param *sb       The pointer to subblock.
 */
static inline void encode_subblock(polynomial_predictor* poly, sslms* sslms, sub_block* sb) {
    uint16_t offset;
    int32_t sample, residual;

    for (offset = 0; offset < sb->size; ++offset) {
        /* STEP 1. Polynomial prediction. */
        residual = sb->samples[offset] - polynomial_predictor_predict(poly);
        polynomial_predictor_update(poly, sb->samples[offset]);

        /* STEP 2. SSLMS prediction. */
        sample = residual;
        residual -= sslms_predict(sslms);
        sslms_update(sslms, sample, residual);

        /* STEP 3. Output */
        sb->samples[offset] = residual;
    }
}

/*!
 * @brief           Convert independent stereo to mid-side stereo.
 * @param *block    The pointer to block.
 */
static inline void conv_independent_to_midside(block* block) {
    uint16_t offset;
    int32_t mid, side, left, right;

    if (block->num_channels != 2) {
        return;
    }

    for (offset = 0; offset < block->size; ++offset) {
        left = block->sub_blocks[0]->samples[offset];
        right = block->sub_blocks[1]->samples[offset];

        mid = RSHIFT(left + right, 1);
        side = left - right;

        block->sub_blocks[0]->samples[offset] = mid;
        block->sub_blocks[1]->samples[offset] = side;
    }
}

/*!
 * @brief           Encode current block.
 * @param *encoder  Pointer to encoder.
 */
static inline void encode_current_block(encoder* encoder) {
    uint8_t ch;

    if (encoder->header.use_mid_side_stereo) {
        conv_independent_to_midside(encoder->current_block);
    }

    for (ch = 0; ch < encoder->header.num_channels; ++ch) {
        encode_subblock(encoder->polynomial_predictors[ch], encoder->sslms_filters[ch], encoder->current_block->sub_blocks[ch]);
    }
}

/*!
 * @brief               Calculate the number of blocks.
 * @param num_samples   The number of samples.
 * @param num_channels  The number of channels.
 * @param block_size    The number of block size.
 * @return              The number of blocks.
 */
static inline uint32_t compute_block_count(uint32_t num_samples, uint8_t num_channels, uint16_t block_size) {
    uint32_t num_samples_per_ch = num_samples / num_channels;
    uint32_t num_blocks = num_samples_per_ch / block_size;

    if (num_samples_per_ch % block_size != 0) {
        ++num_blocks;
    }

    return num_blocks;
}

#pragma warning(push)
#pragma warning(disable: 6011)

/*!
 * @brief                           Initialize encoder.
 * @param *encoder                  Pointer to encoder.
 * @param *file                     Output file.
 * @param sample_rate               Sample rate.
 * @param bits_per_sample           Quantization bits.
 * @param num_channels              The number of channels.
 * @param num_samples               The number of samples.
 * @param block_size                Block size.
 * @param use_mid_side_stereo       Mid-Side stereo flag.
 * @param filter_taps               The taps of SSLMS filter.
 * @param output_format_version     Output format version.
 */
static inline void init(
    encoder* encoder, 
    FILE* file, 
    uint32_t sample_rate, 
    uint8_t bits_per_sample, 
    uint8_t num_channels, 
    uint32_t num_samples, 
    uint16_t block_size, 
    bool use_mid_side_stereo, 
    uint8_t filter_taps,
    uint8_t output_format_version) {
    uint8_t ch;

    if (filter_taps > SSLMS_MAX_TAPS) {
        filter_taps = SSLMS_MAX_TAPS;
    }

    if (block_size == 0) {
        switch (sample_rate) {
        case 8000:
        case 11025:
        case 22050:
        case 32000:
            block_size = 1024;
            break;
        case 44100:
        case 48000:
            block_size = 2048;
            break;
        default:
            block_size = 1024;
            break;
        }
    }

    zilophix_header_init(&encoder->header);
    encoder->header.format_version = output_format_version;
    encoder->header.sample_rate = sample_rate;
    encoder->header.bits_per_sample = bits_per_sample;
    encoder->header.num_channels = num_channels;
    encoder->header.num_samples = num_samples;
    encoder->header.filter_taps = filter_taps;
    encoder->header.block_size = block_size;
    encoder->header.use_mid_side_stereo = use_mid_side_stereo;
    encoder->header.num_blocks = compute_block_count(num_samples, num_channels, block_size);
    encoder->header.audio_data_offset = DUMMY_VALUE;
    encoder->header.audio_data_size = DUMMY_VALUE;

    /* If SSLMS filter shift coefficient is not specified, use default coefficients. */
    if (encoder->header.sslms_shift == 0) {
        encoder->header.sslms_shift = zilophix_get_sslms_shift_coefficient(filter_taps, output_format_version, bits_per_sample);
    }

    encoder->output_file = file;
    encoder->output_bit_stream = bit_stream_create(encoder->output_file, BIT_STREAM_MODE_WRITE);
    encoder->sslms_filters = (sslms**)malloc(sizeof(sslms*) * num_channels);
    encoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * num_channels);
    encoder->coder = code_create(encoder->output_bit_stream, output_format_version, bits_per_sample);
    encoder->current_block = (block*)malloc(sizeof(block));
    encoder->current_sub_block_channel = 0;
    encoder->current_sub_block_offset = 0;

    /* Initialize block */
    block_init(encoder->current_block, block_size, num_channels);

    /* Initialize all filters. */
    for (ch = 0; ch < num_channels; ++ch) {
        encoder->sslms_filters[ch] = sslms_create(filter_taps, encoder->header.sslms_shift);
        encoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }

    /* Write header */
    write_header(encoder);

    encoder->audio_data_begin_position = (uint32_t)ftell(encoder->output_file);
}

#pragma warning(pop)

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
 * @param output_format_version     Output format version.
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
    uint8_t output_format_version) {
    encoder* result = (encoder*)malloc(sizeof(encoder));

    if (result == NULL) {
        report_error(ERROR_ENCODER_CANNOT_ALLOCATE_MEMORY);
        return NULL;
    }

    init(
        result,
        file,
        sample_rate,
        bits_per_sample,
        num_channels,
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        output_format_version);

    return result;
}

/*!
 * @brief           Release encoder.
 * @param *encoder  Pointer to encoder.
 */
void encoder_free(encoder* encoder) {
    uint8_t ch;

    /* Release all filters. */
    for (ch = 0; ch < encoder->header.num_channels; ++ch) {
        sslms_free(encoder->sslms_filters[ch]);
        polynomial_predictor_free(encoder->polynomial_predictors[ch]);
    }

    free(encoder->output_bit_stream);
    free(encoder->sslms_filters);
    free(encoder->polynomial_predictors);
    free(encoder->coder);

    block_free(encoder->current_block);
    code_free(encoder->coder);
}

/*!
 * @brief           Encode sample.
 * @param *encoder  Pointer to encoder.
 * @param sample    Sample
 */
void encoder_write_sample(encoder* encoder, int32_t sample) {
    int32_t flg_encode_block = 0;

    encoder->current_block->sub_blocks[encoder->current_sub_block_channel++]->samples[encoder->current_sub_block_offset] = sample;

    /* Update offsets. */
    if (encoder->current_sub_block_channel >= encoder->header.num_channels) {
        encoder->current_sub_block_channel = 0;
        ++encoder->current_sub_block_offset;

        if (encoder->current_sub_block_offset >= encoder->header.block_size) {
            encoder->current_sub_block_offset = 0;
            flg_encode_block = 1;
        }
    }

    if (flg_encode_block == 1) {
        encode_current_block(encoder);
        code_write_block(encoder->coder, encoder->current_block);
    }
}

/*!
 * @brief           End encode.
 * @param *encoder  Pointer to encoder.
 */
void encoder_end_write(encoder* encoder) {
    if (encoder->current_sub_block_offset != 0) {
        encode_current_block(encoder);
        code_write_block(encoder->coder, encoder->current_block);
    }

    bit_stream_close(encoder->output_bit_stream);

    flush_audio_data(encoder);
    fflush(encoder->output_file);
    fclose(encoder->output_file);
}