#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/decoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include "./include/zilophix.h"

/*! 
 * @brief           Check version
 * @param version   Version number
 * @return          Returns true if the version is supported, false if the version is not supported.
 */
static inline bool is_supported_version(uint8_t version) {
    if (version <= FORMAT_VERSION_CURRENT) {
        return true;
    }

    return false;
}

/*!
 * @brief           Check magic number.
 * @param *decoder  Pointer to decoder.
 * @return          Returns true if the opened file is in ZilophiX format, false if the file is in any other format.
 */
static inline bool check_magic_number(decoder* decoder){
    return  read_uint8(decoder->file) == 0x5A &&
            read_uint8(decoder->file) == 0x70 &&
            read_uint8(decoder->file) == 0x58;
}

/*!
 * @brief           Read format informations.
 * @param *decoder  Pointer to decoder.
 */
static inline void read_format_info(decoder* decoder){
    read_mem(decoder->file, &decoder->header, sizeof(zilophix_header));

    if (!is_supported_version(decoder->header.format_version)) {
        report_error(ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION);
    }
}

/*!
 * @brief           Read ZilophiX file header.
 * @param *decoder  Pointer to decoder.
 */
static inline void read_header(decoder* decoder) {
    if (check_magic_number(decoder)){
        read_format_info(decoder);
    }
    else {
        report_error(ERROR_DECODER_INVALID_MAGIC_NUMBER);
    }
}

/*!
 * @brief           Decode subblock.
 * @param *poly     The pointer to polynomial predictor.
 * @param *sslms    The pointer to SSLMS filter.
 * @param *sb       The pointer to subblock.
 */
static inline void decode_sub_block(polynomial_predictor* poly, sslms* sslms, sub_block* sb) {
    uint16_t offset;
    int32_t residual;
    int32_t sample;

    for (offset = 0; offset < sb->size; ++offset) {
        /* STEP 1. Restore polynomial filter prediction residual. */
        residual = sb->samples[offset];
        sample = residual + sslms_predict(sslms);
        sslms_update(sslms, sample, residual);

        /* STEP 2. Polynomial prediction and restore PCM sample. */
        sample += polynomial_predictor_predict(poly);
        polynomial_predictor_update(poly, sample);

        /* STEP 3. Store restored sample to subblock. */
        sb->samples[offset] = sample;
    }
}

/*!
 * @brief           Convert mid-side stereo to independent stereo.
 * @param *block    The pointer to block.
 */
static inline void conv_midside_to_independent(block* block) {
    uint16_t offset;
    int32_t mid, side, left, right;

    if (block->num_channels != 2) {
        return;
    }

    for (offset = 0; offset < block->size; ++offset) {
        mid = block->sub_blocks[0]->samples[offset];
        side = block->sub_blocks[1]->samples[offset];

        mid = LSHIFT(mid, 1) | (side & 1);
        left = RSHIFT(mid + side, 1);
        right = RSHIFT(mid - side, 1);

        block->sub_blocks[0]->samples[offset] = left;
        block->sub_blocks[1]->samples[offset] = right;
    }
}

/*!
 * @brief           Decode current block.
 * @param *decoder  Pointer to decoder.
 */
static inline void decode_current_block(decoder* decoder) {
    uint8_t ch;

    for (ch = 0; ch < decoder->header.num_channels; ++ch) {
        decode_sub_block(decoder->polynomial_predictors[ch], decoder->sslms_filters[ch], decoder->current_block->sub_blocks[ch]);
    }

    if (decoder->header.use_mid_side_stereo) {
        conv_midside_to_independent(decoder->current_block);
    }
}

#pragma warning(push)
#pragma warning(disable: 6011)

/*!
 * @brief           Initialization decoder.
 * @param *decoder  Pointer to decoder.
 * @param *file     file.
 */
static inline void decoder_init(decoder* decoder, FILE* file) {
    uint8_t ch;

    /* Initialize bitstream */
    decoder->file = file;
    decoder->bit_stream = bit_stream_create(decoder->file, BIT_STREAM_MODE_READ);

    /* Read header */
    read_header(decoder);

    decoder->sslms_filters = (sslms**)malloc(sizeof(sslms*) * decoder->header.num_channels);
    decoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * decoder->header.num_channels);
    decoder->coder = code_create(decoder->bit_stream, decoder->header.format_version, decoder->header.bits_per_sample);
    decoder->current_block = (block*)malloc(sizeof(block));
    decoder->current_read_sub_block_channel = 0;
    decoder->current_read_sub_block_offset = 0;
    decoder->num_samples_read = 0;
    decoder->is_seeking = false;

    /* Initialize blocks */
    block_init(decoder->current_block, decoder->header.block_size, decoder->header.num_channels);

    /* If the format version is 1.2 or earlier, use default SSLMS shift coefficient for format version 1.2. */
    if (decoder->header.format_version <= FORMAT_VERSION_1_2) {
        decoder->header.sslms_shift = zilophix_get_sslms_shift_coefficient(
            decoder->header.filter_taps,
            FORMAT_VERSION_1_2,
            decoder->header.bits_per_sample
        );
    }

    /* Initialize SSLMS filters and polynomial predictors. */
    for (ch = 0; ch < decoder->header.num_channels; ++ch) {
        decoder->sslms_filters[ch] = sslms_create(decoder->header.filter_taps, decoder->header.sslms_shift);
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
    fseek(result->file, result->header.audio_data_offset, SEEK_SET);
    
    return result;
}

#pragma warning(pop)

/*!
 * @brief                   Release specified decoder.
 * @param decoder           Pointer to decoder.
 */
void decoder_free(decoder* decoder) {
    uint8_t ch;

    /* Release all filters. */
    for (ch = 0; ch < decoder->header.num_channels; ++ch) {
        sslms_free(decoder->sslms_filters[ch]);
        polynomial_predictor_free(decoder->polynomial_predictors[ch]);
    }

    free(decoder->bit_stream);
    free(decoder->sslms_filters);
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
    fclose(decoder->file);
}

/*!
 * @brief                   Force read next sample and return as PCM.
 * @param decoder           Pointer to decoder.
 * @return                  Sample
 */
static inline int32_t force_read_sample(decoder* decoder) {
    int32_t sample;

    if (decoder->current_read_sub_block_offset == 0 && decoder->current_read_sub_block_channel == 0) {
        code_read_block(decoder->coder, decoder->current_block);
        decode_current_block(decoder);
    }

    sample = (int32_t)decoder->current_block->sub_blocks[decoder->current_read_sub_block_channel++]->samples[decoder->current_read_sub_block_offset];

    if (decoder->current_read_sub_block_channel == decoder->header.num_channels) {
        decoder->current_read_sub_block_channel = 0;
        ++decoder->current_read_sub_block_offset;

        if (decoder->current_read_sub_block_offset == decoder->header.block_size) {
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

    if (decoder->num_samples_read >= decoder->header.num_samples) {
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
        fseek(decoder->file, decoder->header.audio_data_offset, SEEK_SET);
        bit_stream_init(decoder->bit_stream);

        /* Initialize block. */
        decoder->current_read_sub_block_channel = 0;
        decoder->current_read_sub_block_offset = 0;
        decoder->num_samples_read = 0;

        /* Initialize all filters. */
        for (ch = 0; ch < decoder->header.num_channels; ++ch) {
            sslms_clear(decoder->sslms_filters[ch]);
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
    uint32_t samples_per_ms = (decoder->header.sample_rate * decoder->header.num_channels) / 1000;
    uint32_t offset = ms * samples_per_ms;

    decoder_seek_sample_to(decoder, offset);
}

/*!
 * @brief                   Get playback duration in milliseconds.
 * @param *decoder          Pointer to decoder.
 * @return                  Playback duration in milliseconds.
 */
uint32_t decoder_get_duration_ms(decoder* decoder) {
    uint32_t samples_per_ms = (decoder->header.sample_rate * decoder->header.num_channels) / 1000;
    uint32_t total_ms = decoder->header.num_samples / samples_per_ms;

    return total_ms;
}