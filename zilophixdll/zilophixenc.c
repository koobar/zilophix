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
    uint8_t filter_taps) {
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
        filter_taps);
}

HENCODER CreateEncoderFromPathW(
    LPCWSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps) {
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
        filter_taps);
}

HENCODER CreateEncoderFromFile(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps) {
    set_on_error_exit(false);
    return encoder_create(
        file,
        sample_rate,
        bits_per_sample,
        num_channels,
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps);
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
    
    /* Open input WAV file. */
    wave_file_reader* reader = wave_file_reader_create(input);
    uint32_t num_samples = wave_file_reader_get_num_samples(reader);
    
    /* Create encoder instance. */
    encoder* encoder = encoder_create(
        fopen(output, "wb"),
        wave_file_reader_get_sample_rate(reader),
        (uint8_t)wave_file_reader_get_bits_per_sample(reader),
        (uint8_t)wave_file_reader_get_num_channels(reader),
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps);

    /* Encode all samples. */
    for (i = 0; i < num_samples; ++i){
        encoder_write_sample(encoder, wave_file_reader_read_sample(reader));
    }
    encoder_end_write(encoder);

    /* Release resources. */
    wave_file_reader_close(reader);
    encoder_free(encoder);
}

void __declspec(dllexport) EncodeFileW(LPCWSTR input, LPCWSTR output, uint16_t block_size, bool use_mid_side_stereo, uint8_t filter_taps) {
    uint32_t i;

    /* Open input WAV file. */
    FILE* wav_file = _wfopen(input, L"rb");
    wave_file_reader* reader = wave_file_reader_create_file(wav_file);
    uint32_t num_samples = wave_file_reader_get_num_samples(reader);

    /* Create encoder instance. */
    encoder* encoder = encoder_create(
        _wfopen(output, L"wb"),
        wave_file_reader_get_sample_rate(reader),
        (uint8_t)wave_file_reader_get_bits_per_sample(reader),
        (uint8_t)wave_file_reader_get_num_channels(reader),
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps);

    /* Encode all samples. */
    for (i = 0; i < num_samples; ++i) {
        encoder_write_sample(encoder, wave_file_reader_read_sample(reader));
    }
    encoder_end_write(encoder);

    /* Release resources. */
    wave_file_reader_close(reader);
    encoder_free(encoder);
}
