#include "./include/zilophixdec.h"
#include "errcode.h"
#include "wave_file_writer.h"
#include <stdio.h>
#include <wchar.h>

HDECODER ZpXCreateDecoderA(LPCSTR path) {
    FILE* file = NULL;

    set_on_error_exit(false);
    file = fopen(path, "rb");

    return decoder_create(file);
}

HDECODER ZpXCreateDecoderW(LPCWSTR path) {
    FILE* file = NULL;

    set_on_error_exit(false);
    file = _wfopen(path, L"rb");

    return decoder_create(file);
}

void ZpXCloseFile(HDECODER decoder) {
    if (decoder != NULL) {
        decoder_close(decoder);
    }
}

void ZpXFreeDecoder(HDECODER decoder) {
    if (decoder != NULL) {
        decoder_free(decoder);
        set_on_error_exit(true);
    }
}

uint32_t ZpXGetSampleRate(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.sample_rate;
}

uint32_t ZpXGetChannels(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return (uint32_t)decoder->header.num_channels;
}

uint32_t ZpXGetBitsPerSample(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return (uint32_t)decoder->header.bits_per_sample;
}

uint32_t ZpXGetNumTotalSamples(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.num_samples;
}

uint16_t ZpXGetBlockSize(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.block_size;
}

uint32_t ZpXGetNumBlocks(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.num_blocks;
}

bool ZpXGetUseMidSideStereo(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->header.use_mid_side_stereo;
}

int ZpXReadSample(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder_read_sample(decoder);
}

uint32_t ZpXGetSampleOffset(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder->num_samples_read;
}

void ZpXSetSampleOffset(HDECODER decoder, uint32_t offset) {
    if (decoder == NULL) {
        return;
    }

    decoder_seek_sample_to(decoder, offset);
}

void ZpXSeekTo(HDECODER decoder, uint32_t msec) {
    if (decoder == NULL) {
        return;
    }

    decoder_seek_milliseconds_to(decoder, msec);
}

uint32_t ZpXGetDurationMsec(HDECODER decoder) {
    if (decoder == NULL) {
        return 0;
    }

    return decoder_get_duration_ms(decoder);
}

void ZpXDecodeFileA(LPCSTR input, LPCSTR output){
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

void ZpXDecodeFileW(LPCWSTR input, LPCWSTR output) {
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