#include "./include/zilophixenc.h"
#include "wave_file_reader.h"
#include "errcode.h"
#include <stdio.h>

HENCODER CreateEncoderFromPathA(
    LPCSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag) {
    FILE* output_file = NULL;

    set_on_error_exit(false);
    output_file = fopen(output, "wb");

    return encoder_create(
        output_file,
        sample_rate,
        bits_per_sample,
        num_channels,
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        tag);
}

HENCODER CreateEncoderFromPathW(
    LPCWSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag) {
    FILE* file;
    
    set_on_error_exit(false);
    file = _wfopen(output, L"wb");

    return encoder_create(
        file,
        sample_rate,
        bits_per_sample,
        num_channels,
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        tag);
}

HENCODER CreateEncoderFromFile(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag) {
    set_on_error_exit(false);
    return encoder_create(
        file,
        sample_rate,
        bits_per_sample,
        num_channels,
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        tag);
}

void FreeEncoder(HENCODER encoder) {
    encoder_free(encoder);
    set_on_error_exit(true);
}

void EncoderWriteSample(HENCODER encoder, int sample) {
    encoder_write_sample(encoder, sample);
}

void EncoderEndWrite(HENCODER encoder) {
    encoder_end_write(encoder);
}

void EncodeFileA(LPCSTR input, LPCSTR output, uint16_t block_size, bool use_mid_side_stereo, uint8_t filter_taps){
    uint32_t i;
    wave_file_reader* reader = wave_file_reader_create(input);
    uint32_t num_samples = wave_file_reader_get_num_samples(reader);
    FILE* output_file = fopen(output, "wb");
    encoder* encoder = encoder_create(
        output_file,
        wave_file_reader_get_sample_rate(reader),
        (uint8_t)wave_file_reader_get_bits_per_sample(reader),
        (uint8_t)wave_file_reader_get_num_channels(reader),
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        NULL);

    /* Encode all samples. */
    for (i = 0; i < num_samples; ++i){
        encoder_write_sample(encoder, wave_file_reader_read_sample(reader));
    }
    encoder_end_write(encoder);

    /* Release resources. */
    wave_file_reader_close(reader);
    encoder_free(encoder);
}