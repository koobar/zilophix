#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/decoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"

const static uint8_t supported_format_versions[2] = { 0x01, 0x10 };

/*! 
 * @brief           Check version
 * @param version   Version number
 * @return          Returns true if the version is supported, false if the version is not supported.
 */
static bool is_supported_version(uint8_t version) {
    size_t n = sizeof(supported_format_versions) / sizeof(supported_format_versions[0]);
    size_t i;

    for (i = 0; i < n; ++i) {
        if (supported_format_versions[i] == version) {
            return true;
        }
    }

    return false;
}

/*!
 * @brief           Check magic number.
 * @param *decoder  Pointer to decoder.
 * @return          Returns true if the opened file is in ZilophiX format, false if the file is in any other format.
 */
static bool check_magic_number(decoder* decoder){
    return  read_uint8(decoder->file) == 0x5A &&
            read_uint8(decoder->file) == 0x70 &&
            read_uint8(decoder->file) == 0x58;
}

/*!
 * @brief           Read format informations.
 * @param *decoder  Pointer to decoder.
 */
static void read_format_info(decoder* decoder){
    /* Read format version */
    decoder->format_version = read_uint8(decoder->file);

    if (!is_supported_version(decoder->format_version)) {
        report_error(ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION);
        return;
    }

    /* Read PCM format informations. */
    decoder->sample_rate = read_uint32(decoder->file);
    decoder->bits_per_sample = read_uint8(decoder->file);
    decoder->num_channels = read_uint8(decoder->file);
    decoder->num_total_samples = read_uint32(decoder->file);

    /* Read Zilophix encode informations. */
    decoder->filter_taps = read_uint8(decoder->file);
    decoder->block_size = read_uint16(decoder->file);
    decoder->use_mid_side_stereo = read_bool(decoder->file);
    decoder->num_blocks = read_uint32(decoder->file);

    /* Read reserved codes. (If unused or unimplemented, it will be zero.) */
    decoder->reserved1 = read_uint8(decoder->file);
    decoder->reserved2 = read_uint8(decoder->file);

    /* Get offset of audio data */
    if (read_char(decoder->file) == 'A' && read_char(decoder->file) == 'D' && read_char(decoder->file) == 'D' && read_char(decoder->file) == 'R'){
        decoder->audio_data_begin = read_uint32(decoder->file);
    }

    /* Get size of audio data. */
    if (read_char(decoder->file) == 'S' && read_char(decoder->file) == 'I' && read_char(decoder->file) == 'Z' && read_char(decoder->file) == 'E'){
        decoder->audio_data_size = read_uint32(decoder->file);
    }
}

/*!
 * @brief           Read ZilophiX file header.
 * @param *decoder  Pointer to decoder.
 */
static void read_header(decoder* decoder) {
    if (check_magic_number(decoder)){
        read_format_info(decoder);
        tag_read(decoder->file, &decoder->tag);
    }
    else {
        report_error(ERROR_DECODER_INVALID_MAGIC_NUMBER);
    }
}

/*!
 * @brief           Decode monaural or no Mid-Side stereo block.
 * @param *decoder  Pointer to decoder.
 */
static void decode_normal_block(decoder* decoder){
    uint8_t ch;
    sub_block* sb = NULL;
    lms* lms = NULL;
    polynomial_predictor* poly = NULL;
    uint16_t offset;
    int32_t residual;
    int32_t sample;

    for (ch = 0; ch < decoder->num_channels; ++ch) {
        sb = decoder->current_block->sub_blocks[ch];
        lms = decoder->lms_filters[ch];
        poly = decoder->polynomial_predictors[ch];

        for (offset = 0; offset < sb->size; ++offset) {
            /* STEP 1. Restore polynomial filter prediction residual. */
            residual = sb->samples[offset];
            sample = residual + lms_predict(lms);
            lms_update(lms, sample, residual);

            /* STEP 2. Polynomial prediction and restore PCM sample. */
            sample += polynomial_predictor_predict(poly);
            polynomial_predictor_update(poly, sample);

            /* STEP 3. Store restored sample to subblock. */
            sb->samples[offset] = sample;
        }
    }
}

/*!
 * @brief           Decode Mid-Side stereo block.
 * @param *decoder  Pointer to decoder.
 */
static void decode_midside_block(decoder* decoder){
    sub_block* lch = decoder->current_block->sub_blocks[0];
    sub_block* rch = decoder->current_block->sub_blocks[1];
    lms* llms = decoder->lms_filters[0];
    lms* rlms = decoder->lms_filters[1];
    polynomial_predictor* lpoly = decoder->polynomial_predictors[0];
    polynomial_predictor* rpoly = decoder->polynomial_predictors[1];
    int32_t left;
    int32_t right;
    int32_t mid;
    int32_t side;
    uint16_t offset;

    for (offset = 0; offset < decoder->block_size; ++offset) {
        /* STEP 1. Restore polynomial filter prediction residuals. */
        mid = lch->samples[offset] + lms_predict(llms);
        side = rch->samples[offset] + lms_predict(rlms);
        lms_update(llms, mid, lch->samples[offset]);
        lms_update(rlms, side, rch->samples[offset]);

        /* STEP 2. Polynomial prediction and restore Mid-Side stereo PCM samples.*/
        mid += polynomial_predictor_predict(lpoly);
        side += polynomial_predictor_predict(rpoly);
        polynomial_predictor_update(lpoly, mid);
        polynomial_predictor_update(rpoly, side);

        /* STEP 3. Convert Mid-Side stereo to simple stereo. */
        mid = LSHIFT(mid, 1) | (side & 1);
        left = RSHIFT(mid + side, 1);
        right = RSHIFT(mid - side, 1);

        /* STEP 4. Store left and right sample to subblocks. */
        lch->samples[offset] = left;
        rch->samples[offset] = right;
    }
}

/*!
 * @brief           Decode current block.
 * @param *decoder  Pointer to decoder.
 */
static void decode_current_block(decoder* decoder) {
    if (decoder->num_channels == 2 && decoder->use_mid_side_stereo){
        decode_midside_block(decoder);
    }
    else{
        decode_normal_block(decoder);
    }
}

/*!
 * @brief           Initialization decoder.
 * @param *decoder  Pointer to decoder.
 * @param *file     file.
 */
static void decoder_init(decoder* decoder, FILE* file) {
    uint8_t ch;

    /* Initialize bitstream */
    decoder->file = file;
    decoder->bit_stream = bit_stream_create(decoder->file, BIT_STREAM_MODE_READ);

    /* Read header */
    read_header(decoder);

    decoder->lms_filters = (lms**)malloc(sizeof(lms*) * decoder->num_channels);
    decoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * decoder->num_channels);
    decoder->coder = code_create(decoder->bit_stream, decoder->format_version, decoder->bits_per_sample);
    decoder->current_block = (block*)malloc(sizeof(block));
    decoder->current_read_sub_block_channel = 0;
    decoder->current_read_sub_block_offset = 0;
    decoder->num_samples_read = 0;
    decoder->is_seeking = false;

    /* Initialize blocks */
    block_init(decoder->current_block, decoder->block_size, decoder->num_channels);

    /* Initialize SSLMS filters and polynomial predictors. */
    for (ch = 0; ch < decoder->num_channels; ++ch) {
        decoder->lms_filters[ch] = lms_create(decoder->filter_taps, decoder->bits_per_sample);
        decoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }
}

/*!
 * @brief                   Create new instance of decoder.
 * @param file*             Pointer to file.
 * @return                  Pointer to created instance.
 */
decoder* decoder_create(FILE* file) {
    decoder* result = (decoder*)malloc(sizeof(decoder));
    decoder_init(result, file);
    
    /* Seek to audio data begin offset. */
    fseek(result->file, result->audio_data_begin, SEEK_SET);
    
    return result;
}

/*!
 * @brief                   Release specified decoder.
 * @param decoder           Pointer to decoder.
 */
void decoder_free(decoder* decoder) {
    uint8_t ch;

    /* Release all filters. */
    for (ch = 0; ch < decoder->num_channels; ++ch) {
        lms_free(decoder->lms_filters[ch]);
        polynomial_predictor_free(decoder->polynomial_predictors[ch]);
    }

    free(decoder->bit_stream);
    free(decoder->lms_filters);
    free(decoder->polynomial_predictors);
    free(decoder->coder);
    block_free(decoder->current_block);
    code_free(decoder->coder);
}

/*!
 * @brief                   Close decoding file.
 * @param decoder           Pointer to decoder.
 */
void decoder_close(decoder* decoder) {
    tag_free(decoder->tag);
    fclose(decoder->file);
}

/*!
 * @brief                   Force read next sample and return as PCM.
 * @param decoder           Pointer to decoder.
 * @return                  Sample
 */
static int32_t force_read_sample(decoder* decoder) {
    int32_t sample;

    if (decoder->current_read_sub_block_offset == 0 && decoder->current_read_sub_block_channel == 0) {
        code_read_block(decoder->coder, decoder->current_block);
        decode_current_block(decoder);
    }

    sample = (int32_t)decoder->current_block->sub_blocks[decoder->current_read_sub_block_channel++]->samples[decoder->current_read_sub_block_offset];

    if (decoder->current_read_sub_block_channel == decoder->num_channels) {
        decoder->current_read_sub_block_channel = 0;
        ++decoder->current_read_sub_block_offset;

        if (decoder->current_read_sub_block_offset == decoder->block_size) {
            decoder->current_read_sub_block_offset = 0;
        }
    }
    decoder->num_samples_read += 1;

    return sample;
}

/*!
 * @brief                   Read next sample and return as PCM.
 * @param decoder           Pointer to decoder.
 * @return                  Sample
 */
int32_t decoder_read_sample(decoder* decoder) {
    if (decoder->is_seeking) {
        return 0;
    }

    if (decoder->num_samples_read >= decoder->num_total_samples) {
        return 0;
    }

    return force_read_sample(decoder);
}

/*!
 * @brief                   Seek to specified sample offset.
 * @param *decoder          Pointer to decoder.
 * @param sample_offset     Sample offset.
 */
void decoder_seek_sample_to(decoder* decoder, uint32_t sample_offset) {
    uint8_t ch;
    uint32_t offset;

    decoder->is_seeking = true;

    /* In the case of a backward seek (moving the playback position back to a previous point),
       decoding is restarted from the beginning of the file. */
    if (sample_offset < decoder->num_samples_read) {
        fseek(decoder->file, decoder->audio_data_begin, SEEK_SET);
        bit_stream_init(decoder->bit_stream);

        /* Initialize block. */
        decoder->current_read_sub_block_channel = 0;
        decoder->current_read_sub_block_offset = 0;
        decoder->num_samples_read = 0;

        /* Initialize all filters. */
        for (ch = 0; ch < decoder->num_channels; ++ch) {
            lms_clear(decoder->lms_filters[ch]);
            polynomial_predictor_clear(decoder->polynomial_predictors[ch]);
        }
        offset = 0;
    }
    else {
        offset = decoder->num_samples_read;
    }

    /* Skip to the specified offset. */
    for (; offset < sample_offset; ++offset) {
        force_read_sample(decoder);
    }

    decoder->is_seeking = false;
}

/*!
 * @brief                   Seek to specific time in milliseconds.
 * @param *decoder          Pointer to decoder.
 * @param sample_offset     Seek destination time in milliseconds.
 */
void decoder_seek_milliseconds_to(decoder* decoder, uint32_t ms) {
    uint32_t samples_per_ms = (decoder->sample_rate * decoder->num_channels) / 1000;
    uint32_t offset = ms * samples_per_ms;

    decoder_seek_sample_to(decoder, offset);
}

/*!
 * @brief                   Get playback duration in milliseconds.
 * @param *decoder          Pointer to decoder.
 * @return                  Playback duration in milliseconds.
 */
uint32_t decoder_get_duration_ms(decoder* decoder) {
    uint32_t samples_per_ms = (decoder->sample_rate * decoder->num_channels) / 1000;
    uint32_t total_ms = decoder->num_total_samples / samples_per_ms;

    return total_ms;
}