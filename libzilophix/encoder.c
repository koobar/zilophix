#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/zilophix.h"
#include "./include/encoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include <stdlib.h>
#include <string.h>

#define DUMMY_VALUE                     0xFFFFFFFF  

/*!
 * @brief           Write magic numbers
 * @param *encoder  Pointer to encoder.
*/
static void write_magic_number(encoder* encoder){
    write_uint8(encoder->output_file, 0x5A);
    write_uint8(encoder->output_file, 0x70);
    write_uint8(encoder->output_file, 0x58);
}

/*!
 * @brief           Write header.
 * @param *encoder  Pointer to encoder.
 */
static void write_header(encoder* encoder) {
    write_magic_number(encoder);
    write_mem(encoder->output_file, &encoder->header, sizeof(zilophix_header));
}

/*!
 * @brief           Write audio data offset and audio data size.
 * @param *encoder  Pointer to encoder.
*/
static void flush_audio_data(encoder* encoder){
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
 * @brief           Encode monaural or normal stereo block. 
 * @param *encoder  Pointer to encoder
 */
static void encode_current_block_normal(encoder* encoder){
    uint8_t ch;
    uint16_t offset;
    int32_t sample, residual;
    sub_block* sb = NULL;
    polynomial_predictor* poly = NULL;
    sslms* lms = NULL;

    for (ch = 0; ch < encoder->header.num_channels; ++ch) {
        sb = encoder->current_block->sub_blocks[ch];
        poly = encoder->polynomial_predictors[ch];
        lms = encoder->sslms_filters[ch];

        for (offset = 0; offset < sb->size; ++offset) {
            /* STEP 1. Polynomial prediction. */
            residual = sb->samples[offset] - polynomial_predictor_predict(poly);
            polynomial_predictor_update(poly, sb->samples[offset]);

            /* STEP 2. SSLMS prediction. */
            sample = residual;
            residual -= sslms_predict(lms);
            sslms_update(lms, sample, residual);

            /* STEP 3. Output */
            sb->samples[offset] = residual;
        }
    }
}

/*!
 * @brief           Encode Mid-Side stereo block.
 * @param *encoder  Pointer to encoder.
 */
static void encode_current_block_midside(encoder* encoder){
    uint16_t offset;
    int32_t left;
    int32_t right;
    int32_t mid;
    int32_t side;
    sub_block* lch = encoder->current_block->sub_blocks[0];
    sub_block* rch = encoder->current_block->sub_blocks[1];
    polynomial_predictor* lpoly = encoder->polynomial_predictors[0];
    polynomial_predictor* rpoly = encoder->polynomial_predictors[1];
    sslms* llms = encoder->sslms_filters[0];
    sslms* rlms = encoder->sslms_filters[1];

    for (offset = 0; offset < encoder->header.block_size; ++offset){
        left = lch->samples[offset];
        right = rch->samples[offset];

        /* STEP 1. Convert to Mid-Side stereo. */
        lch->samples[offset] = mid = RSHIFT(left + right, 1);
        rch->samples[offset] = side = left - right;

        /* STEP 2. Polynomial prediction. */
        mid -= polynomial_predictor_predict(lpoly);
        side -= polynomial_predictor_predict(rpoly);
        polynomial_predictor_update(lpoly, lch->samples[offset]);
        polynomial_predictor_update(rpoly, rch->samples[offset]);
        lch->samples[offset] = mid;
        rch->samples[offset] = side;

        /* STEP 3. SSLMS prediction. */
        mid -= sslms_predict(llms);
        side -= sslms_predict(rlms);
        sslms_update(llms, lch->samples[offset], mid);
        sslms_update(rlms, rch->samples[offset], side);

        /* STEP 4. Output */
        lch->samples[offset] = mid;
        rch->samples[offset] = side;
    }
}

/*!
 * @brief           Encode current block.
 * @param *encoder  Pointer to encoder.
 */
static void encode_current_block(encoder* encoder) {
    if (encoder->header.use_mid_side_stereo){
        encode_current_block_midside(encoder);
    }
    else{
        encode_current_block_normal(encoder);
    }
}

/*!
 * @brief               Calculate the number of blocks.
 * @param num_samples   The number of samples.
 * @param num_channels  The number of channels.
 * @param block_size    The number of block size.
 * @return              The number of blocks.
 */
static uint32_t compute_block_count(uint32_t num_samples, uint8_t num_channels, uint16_t block_size) {
    uint32_t num_samples_per_ch = num_samples / num_channels;
    uint32_t num_blocks = num_samples_per_ch / block_size;

    if (num_samples_per_ch % block_size != 0) {
        ++num_blocks;
    }

    return num_blocks;
}

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
 */
static void init(
    encoder* encoder, 
    FILE* file, 
    uint32_t sample_rate, 
    uint8_t bits_per_sample, 
    uint8_t num_channels, 
    uint32_t num_samples, 
    uint16_t block_size, 
    bool use_mid_side_stereo, 
    uint8_t filter_taps) {
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
    encoder->header.format_version = FORMAT_VERSION_CURRENT;
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

    encoder->output_file = file;
    encoder->output_bit_stream = bit_stream_create(encoder->output_file, BIT_STREAM_MODE_WRITE);
    encoder->sslms_filters = (sslms**)malloc(sizeof(sslms*) * num_channels);
    encoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * num_channels);
    encoder->coder = code_create(encoder->output_bit_stream, FORMAT_VERSION_CURRENT, bits_per_sample);
    encoder->current_block = (block*)malloc(sizeof(block));
    encoder->current_sub_block_channel = 0;
    encoder->current_sub_block_offset = 0;

    /* Initialize block */
    block_init(encoder->current_block, block_size, num_channels);

    /* Initialize all filters. */
    for (ch = 0; ch < num_channels; ++ch) {
        encoder->sslms_filters[ch] = sslms_create(filter_taps, bits_per_sample);
        encoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }

    /* Write header */
    write_header(encoder);

    encoder->audio_data_begin_position = (uint32_t)ftell(encoder->output_file);
}

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
 */
encoder* encoder_create(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps) {
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
        filter_taps);

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