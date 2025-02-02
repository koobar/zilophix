#include "./include/zilophixdec.h"
#include "errcode.h"
#include "wave_file_writer.h"
#include <stdio.h>
#include <wchar.h>

HDECODER CreateDecoderA(LPCSTR path) {
    FILE* file = NULL;

    set_on_error_exit(false);
    file = fopen(path, "rb");

    return decoder_create(file);
}

HDECODER CreateDecoderW(LPCWSTR path) {
    FILE* file = NULL;

    set_on_error_exit(false);
    file = _wfopen(path, L"rb");

    return decoder_create(file);
}

void DecoderCloseFile(HDECODER decoder) {
    if (decoder != NULL) {
        decoder_close(decoder);
    }
}

void FreeDecoder(HDECODER decoder) {
    if (decoder != NULL) {
        decoder_free(decoder);
        set_on_error_exit(true);
    }
}

uint32_t DecoderGetSampleRate(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.sample_rate;
}

uint32_t DecoderGetChannels(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return (uint32_t)decoder->header.num_channels;
}

uint32_t DecoderGetBitsPerSample(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return (uint32_t)decoder->header.bits_per_sample;
}

uint32_t DecoderGetNumTotalSamples(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.num_samples;
}

uint16_t DecoderGetBlockSize(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.block_size;
}

uint32_t DecoderGetNumBlocks(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.num_blocks;
}

bool DecoderGetUseMidSideStereo(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.use_mid_side_stereo;
}

int DecoderReadSample(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder_read_sample(decoder);
}

uint32_t DecoderGetSampleOffset(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->num_samples_read;
}

void DecoderSetSampleOffset(HDECODER decoder, uint32_t offset) {
    if (decoder == NULL) {
        return;
    }

    decoder_seek_sample_to(decoder, offset);
}

void DecoderSeekTo(HDECODER decoder, uint32_t msec) {
    if (decoder == NULL) {
        return;
    }

    decoder_seek_milliseconds_to(decoder, msec);
}

uint32_t DecoderGetDurationMsec(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder_get_duration_ms(decoder);
}

void DecodeFileA(LPCSTR input, LPCSTR output){
    FILE* input_file = fopen(input, "rb");
    decoder* decoder = decoder_create(input_file);
    wave_file_writer* writer = wave_file_writer_create(output);
    uint32_t i;

    /* Set format of WAV file. */
    wave_file_writer_set_pcm_format(writer, decoder->header.sample_rate, decoder->header.bits_per_sample, decoder->header.num_channels);
    wave_file_writer_set_num_samples(writer, decoder->header.num_samples);

    /* Encode all samples. */
    wave_file_writer_begin_write(writer);
    for (i = 0; i < decoder->header.num_samples; ++i){
        wave_file_writer_write_sample(writer, decoder_read_sample(decoder));
    }
    wave_file_writer_end_write(writer);
    
    /* Release resources. */
    wave_file_writer_close(writer);
    decoder_close(decoder);
    decoder_free(decoder);
}

void DecodeFileW(LPCWSTR input, LPCWSTR output) {
    FILE* input_file = _wfopen(input, L"rb");
    FILE* output_file = _wfopen(output, L"wb");
    decoder* decoder = decoder_create(input_file);
    wave_file_writer* writer = wave_file_writer_create_file(output_file);
    uint32_t i;

    /* Set format of WAV file. */
    wave_file_writer_set_pcm_format(writer, decoder->header.sample_rate, decoder->header.bits_per_sample, decoder->header.num_channels);
    wave_file_writer_set_num_samples(writer, decoder->header.num_samples);

    /* Encode all samples. */
    wave_file_writer_begin_write(writer);
    for (i = 0; i < decoder->header.num_samples; ++i) {
        wave_file_writer_write_sample(writer, decoder_read_sample(decoder));
    }
    wave_file_writer_end_write(writer);

    /* Release resources. */
    wave_file_writer_close(writer);
    decoder_close(decoder);
    decoder_free(decoder);
}