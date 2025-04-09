#include "./include/path.h"
#include "zilophix.h"
#include "decoder.h"
#include "encoder.h"
#include "wave_file_reader.h"
#include "wave_file_writer.h"
#include <stdbool.h>
#include <time.h>
#include <string.h>

static char* input_file_path = NULL;
static char* output_file_path = NULL;
static bool use_mid_side_stereo = false;
static bool is_silent_mode = false;
static bool is_help_mode = false;
static uint16_t block_size = 0;
static uint8_t filter_taps = 4;
static uint8_t output_format_version = FORMAT_VERSION_CURRENT;

/*!
 * @brief               Parse command line arguments.
 * @param argc          The number of arguments.
 * @param argv          Arguments.
 */
static void parse_commandline_args(int argc, char* argv[]) {
    int i;

    for (i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-ms") == 0 || strcmp(argv[i], "-midside") == 0) {
            use_mid_side_stereo = true;
        }
        else if (strcmp(argv[i], "--bs") == 0 || strcmp(argv[i], "--blocksize") == 0) {
            block_size = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--taps") == 0 || strcmp(argv[i], "--filter-taps") == 0) {
            filter_taps = atoi(argv[i + 1]);
            ++i;
        }
        else if (strcmp(argv[i], "--in") == 0 || strcmp(argv[i], "--input") == 0) {
            input_file_path = argv[++i];
        }
        else if (strcmp(argv[i], "--out") == 0 || strcmp(argv[i], "--output") == 0) {
            output_file_path = argv[++i];
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "-silent") == 0) {
            is_silent_mode = true;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0) {
            is_help_mode = true;
        }
        else if (strcmp(argv[i], "--format_version") == 0 || strcmp(argv[i], "--fmtver") == 0) {
            ++i;

            if (strcmp(argv[i], "1.0") == 0) {
                output_format_version = FORMAT_VERSION_1_0;
            }
            else if (strcmp(argv[i], "1.1") == 0) {
                output_format_version = FORMAT_VERSION_1_1;
            }
            else if (strcmp(argv[i], "1.2") == 0) {
                output_format_version = FORMAT_VERSION_1_2;
            }
            else if (strcmp(argv[i], "1.3") == 0) {
                output_format_version = FORMAT_VERSION_1_3;
            }
            else if (strcmp(argv[i], "current") == 0 || strcmp(argv[i], "latest") == 0) {
                output_format_version = FORMAT_VERSION_CURRENT;
            }
        }
    }
}

/*!
 * @brief           Print message
 * @param *str      message.
 */
static void print(const char* str) {
    if (!is_silent_mode) {
        printf("%s", str);
        printf("\n");
    }
}

#pragma warning(push)
#pragma warning(disable: 6387)

/*!
 * @brief           Print versions.
 */
static void print_versions() {
    size_t buffer_size = sizeof(char) * 1024;
    char* buffer = (char*)malloc(buffer_size);

    sprintf(buffer, "Tool version:    %s", CURRENT_LIBRARY_VERSION_NAME);
    print(buffer);

    sprintf(buffer, "Codec version:   %s", CURRENT_FORMAT_VERSION_NAME);
    print(buffer);

    sprintf(buffer, "Build date:      %s", LIBRARY_BUILD_DATE);
    print(buffer);

    free(buffer);
}

#pragma warning(pop)

/*!
 * @brief           Print line separator.
 */
static void print_separator() {
    print("================================================================================");
}

/*!
 * @brief           Print linebreak.
 */
static void print_return() {
    print("");
}

/*!
 * @brief           Print copyright messages.
 */
static void print_logo() {
    print("ZilophiX (ZpX): The free and open-source lossless audio codec.");
    print("Copyright (c) 2024-2025 koobar. Released under WTFPL version 2.");
    print_return();
    print_versions();
    print_return();
    print("To see how to use it, type this:");
    print("  zilophix -h.");
}

/*!
 * @brief           Print help messages
 */
static void print_usage() {
    printf("Usage:      zilophix [options]\n");
    printf("Example:    zilophix --bs 2048 -ms --in <input> --out <output>\n");
    printf("\n");
    printf("Options:\n");
    printf("    --bs|--blocksize            Specify the number of samples per block. (default = 2048)\n");
    printf("    --taps|--filter-taps        Specify the Sign-Sign LMS adaptive filter taps between 1 and 32.\n");
    printf("    --in|--input                Specify the input file path.\n");
    printf("    --out|--output              Specify the output file path.\n");
    printf("    -ms|-midside                Uses mid-side stereo. Compression rates are often improved.\n");
    printf("    -silent|-s                  Don't display any text.\n");
    printf("    --format_version|--fmtver   Specify the encoder output format version. (e.g. --fmtver current)\n");
    printf("    -h|-help                    Display this text.\n");
}

/*!
 * @brief                   Print encode result.
 * @param src               Source file path.
 * @param output            Destination file path.
 * @param encode_start      Start time 
 * @param encode_end        Stop time
 */
static void print_encode_result(clock_t encode_start, clock_t encode_end) {
    size_t buffer_size = sizeof(char) * 1024;
    char* buffer = (char*)malloc(buffer_size);
    fpos_t src_file_size, dst_file_size;

    if (buffer == NULL) {
        return;
    }

    sprintf_s(buffer, buffer_size, "[RESULT]\0");
    print(buffer);

    sprintf_s(buffer, buffer_size, "Elapsed time: %d msec.\0", encode_end - encode_start);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Output path: %s\0", output_file_path);
    print(buffer);

    src_file_size = get_file_size(input_file_path);
    dst_file_size = get_file_size(output_file_path);

    sprintf_s(buffer, buffer_size, "Source file size: %.2f MiB\0", (src_file_size / 1048576.0));
    print(buffer);

    sprintf_s(buffer, buffer_size, "Destination file size: %.2f MiB\0", (dst_file_size / 1048576.0));
    print(buffer);

    sprintf_s(buffer, buffer_size, "Compression rate: %.2f%% of the original file size.\0", ((double)dst_file_size / src_file_size) * 100.0);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Format version: %s\0", zilophix_get_version_name(output_format_version));
    print(buffer);

    free(buffer);
}

/*!
 * @brief                   Print format informations.
 * @param *decoder          Pointer to decoder.
 */
static void print_format_info(decoder* decoder) {
    size_t buffer_size = sizeof(char) * 1024;
    char* buffer = (char*)malloc(buffer_size);

    if (buffer == NULL) {
        return;
    }

    sprintf_s(buffer, buffer_size, "[FORMAT]\0");
    print(buffer);

    sprintf_s(buffer, buffer_size, "Format version:    %s\0", zilophix_get_version_name(decoder->header.format_version));
    print(buffer);

    sprintf_s(buffer, buffer_size, "Sample rate:       %d Hz\0", decoder->header.sample_rate);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Bits per sample:   %d Bits\0", decoder->header.bits_per_sample);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Channels:          %d Channels\0", decoder->header.num_channels);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Mid-Side stereo:   %s\0", decoder->header.use_mid_side_stereo ? "Yes" : "No");
    print(buffer);

    sprintf_s(buffer, buffer_size, "SSLMS filter taps: %d\0", decoder->header.filter_taps);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Block size:        %d Samples\0", decoder->header.block_size);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Total samples:     %d Samples\0", decoder->header.num_samples);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Total blocks:      %d Blocks\0", decoder->header.num_blocks);
    print(buffer);

    free(buffer);
}

/*!
 * @brief                   Print decode result.
 * @param *decoder          Pointer to decoder.
 * @param output            Output path
 * @param decode_start      Start time
 * @param decode_end        Stop time
 */
static void print_decode_result(decoder* decoder, clock_t decode_start, clock_t decode_end) {
    size_t buffer_size = sizeof(char) * 1024;
    char* buffer = (char*)malloc(buffer_size);

    if (buffer == NULL) {
        return;
    }

    sprintf_s(buffer, buffer_size, "[RESULT]\0");
    print(buffer);

    sprintf_s(buffer, buffer_size, "Output: %s\0", output_file_path);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Elapsed time: %d msec.\0", decode_end - decode_start);
    print(buffer);

    print_return();
    
    /* フォーマット情報を出力 */
    print_format_info(decoder);
    
    free(buffer);
}

/*!
 * @brief           Encode
 */
static void encode() {
    FILE* output_file;
    wave_file_reader* reader = NULL;
    encoder* encoder = NULL;
    uint32_t n, i;
    clock_t start, end;

    /* Delete old file. */
    remove(output_file_path);

    /* Create instance of WAV file reader. */
    reader = wave_file_reader_create(input_file_path);

    /* Get the number of samples from WAV file. */
    n = wave_file_reader_get_num_samples(reader);

    /* Create new instance of encoder. */
    output_file = fopen(output_file_path, "wb");
    encoder = encoder_create(
        output_file,
        wave_file_reader_get_sample_rate(reader),
        (uint8_t)wave_file_reader_get_bits_per_sample(reader),
        (uint8_t)wave_file_reader_get_num_channels(reader),
        n,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        output_format_version);

    /* Save start time. */
    start = clock();

    /* Encode all samples. */
    for (i = 0; i < n; ++i) {
        encoder_write_sample(encoder, wave_file_reader_read_sample(reader));
    }
    encoder_end_write(encoder);
    wave_file_reader_close(reader);
    encoder_free(encoder);

    /* Encode stop time. */
    end = clock();

    /* print result */
    print_encode_result(start, end);
}

/*!
 * @brief           Decode
 */
static void decode() {
    FILE* input_file = NULL;
    wave_file_writer* writer = NULL;
    decoder* decoder = NULL;
    clock_t start, end;
    uint32_t i;

    /* Delete old file. */
    remove(output_file_path);

    /* Create new instance of decoder.*/
    input_file = fopen(input_file_path, "rb");
    decoder = decoder_create(input_file);

    /* Create new WAV file writer and set pcm format and the number of samples. */
    writer = wave_file_writer_create(output_file_path);
    wave_file_writer_set_pcm_format(writer, decoder->header.sample_rate, decoder->header.bits_per_sample, decoder->header.num_channels);
    wave_file_writer_set_num_samples(writer, decoder->header.num_samples);

    /* Save start time */
    start = clock();
    wave_file_writer_begin_write(writer);

    /* Decode and write all samples. */
    for (i = 0; i < decoder->header.num_samples; ++i) {
        wave_file_writer_write_sample(writer, decoder_read_sample(decoder));
    }
    wave_file_writer_end_write(writer);
    wave_file_writer_close(writer);

    /* Save stop time. */
    end = clock();

    /* Print decode result. */
    print_decode_result(decoder, start, end);

    /* Release decoder. */
    decoder_free(decoder);
}

int main(int argc, char* argv[]) {
    size_t buffer_size, i;
    char* extension = NULL;
    char* dir = NULL;
    char* name = NULL;

    /* Parse commandline args */
    parse_commandline_args(argc, argv);

    /* Print logo */
    print_logo();

    if (is_help_mode) {
        print_separator();
        print_usage();
    }
    else if (input_file_path != NULL) {
        print_separator();

        /* Get input file extension */
        extension = get_extension(input_file_path);

        if (strcmp(extension, ".wav") == 0) {
            if (output_file_path == NULL) {
                buffer_size = sizeof(char) * MAX_PATH;
                output_file_path = (char*)malloc(buffer_size);

                if (output_file_path != NULL) {
                    for (i = 0; i < buffer_size; ++i) {
                        output_file_path[i] = '\0';
                    }

                    strcpy_s(output_file_path, buffer_size, input_file_path);
                    change_extension(output_file_path, ".zpx");
                }
            }

            /* Encode */
            encode();
        }
        else if (strcmp(extension, ".zpx") == 0) {
            if (output_file_path == NULL) {
                buffer_size = sizeof(char) * MAX_PATH;
                output_file_path = (char*)malloc(buffer_size);

                if (output_file_path != NULL) {
                    for (i = 0; i < buffer_size; ++i) {
                        output_file_path[i] = '\0';
                    }

                    dir = get_directory_name(input_file_path);
                    name = get_file_name_without_extension(input_file_path);

                    /* Create output file name */
                    strcat_s(output_file_path, buffer_size, dir);
                    strcat_s(output_file_path, buffer_size, PATH_SEPARATOR_STR);
                    strcat_s(output_file_path, buffer_size, name);
                    strcat_s(output_file_path, buffer_size, "_decoded.wav\0");

                    free(dir);
                    free(name);
                }
            }

            /* Decode */
            decode();
        }
    }
    else if (argc != 1) {
        print("[Fatal Error] No input file.");
    }
}