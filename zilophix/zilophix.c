#include "./include/path.h"
#include "zilophix.h"
#include "decoder.h"
#include "encoder.h"
#include "wave_file_reader.h"
#include "wave_file_writer.h"
#include <stdbool.h>
#include <time.h>

static char* input_file_path = NULL;
static char* output_file_path = NULL;
static bool use_mid_side_stereo = false;
static bool is_silent_mode = false;
static bool is_help_mode = false;
static uint16_t block_size = 1024;
static uint8_t filter_taps = 4;

static const char* tag_title;
static const char* tag_album;
static const char* tag_artist;
static const char* tag_album_artist;
static const char* tag_subtitle;
static const char* tag_publisher;
static const char* tag_composer;
static const char* tag_songwriter;
static const char* tag_conductor;
static const char* tag_copyright;
static const char* tag_genre;
static const char* tag_comment;
static uint16_t tag_year;
static uint16_t tag_track_number;
static uint16_t tag_track_count;
static uint16_t tag_disc;
static uint16_t tag_rate;
static const char* tag_picture_path;

static bool has_tag = false;

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
        else if (strcmp(argv[i], "--title") == 0) {
            tag_title = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--album") == 0) {
            tag_album = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--artist") == 0) {
            tag_artist = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--album-artist") == 0) {
            tag_album_artist = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--subtitle") == 0) {
            tag_subtitle = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--publisher") == 0) {
            tag_publisher = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--composer") == 0) {
            tag_composer = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--songwriter") == 0) {
            tag_songwriter = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--conductor") == 0) {
            tag_conductor = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--copyright") == 0) {
            tag_copyright = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--genre") == 0) {
            tag_genre = argv[i + 1];
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--year") == 0) {
            tag_year = (uint16_t)atoi(argv[i + 1]);
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--track-number") == 0) {
            tag_track_number = (uint16_t)atoi(argv[i + 1]);
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--track-count") == 0) {
            tag_track_count = (uint16_t)atoi(argv[i + 1]);
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--disc") == 0) {
            tag_disc = (uint16_t)atoi(argv[i + 1]);
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--rate") == 0) {
            tag_rate = (uint16_t)atoi(argv[i + 1]);
            ++i;
            has_tag = true;
        }
        else if (strcmp(argv[i], "--picture") == 0) {
            tag_picture_path = argv[i + 1];
            ++i;
            has_tag = true;
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
    print("Codec version:   1.0");
    print("Tool version:    1.01");
    print("Build:           2025/01/11");
}

/*!
 * @brief           Print help messages
 */
static void print_usage() {
    printf("Usage:      zilophix [options]\n");
    printf("Example:    zilophix --bs 1024 -ms --in <input> --out <output>\n");
    printf("\n");
    printf("Options:\n");
    printf("    --bs|--blocksize            Specify the number of samples per block. (default = 1024)\n");
    printf("    --taps|--filter-taps        Specify the Sign-Sign LMS adaptive filter taps between 1 and 32.\n");
    printf("    --in|--input                Specify the input file path.\n");
    printf("    --out|--output              Specify the output file path.\n");
    printf("    -ms|-midside                Uses mid-side stereo. Compression rates are often improved.\n");
    printf("    -silent|-s                  Don't display any text.\n");
    printf("    --title                     Set the title in tag information.\n");
    printf("    --album                     Set the album in tag information.\n");
    printf("    --artist                    Set the artist in tag information.\n");
    printf("    --album-artist              Set the album artist in tag information.\n");
    printf("    --subtitle                  Set the subtitle in tag information.\n");
    printf("    --publisher                 Set the publisher in tag information.\n");
    printf("    --composer                  Set the composer in tag information.\n");
    printf("    --songwriter                Set the songwriter in tag information.\n");
    printf("    --conductor                 Set the conductor in tag information.\n");
    printf("    --copyright                 Set the copyright notices in tag information.\n");
    printf("    --comment                   Set the comment in tag information.\n");
    printf("    --genre                     Set the genre in tag information.\n");
    printf("    --year                      Set the year in tag information.\n");
    printf("    --track-number              Set the track number in tag information.\n");
    printf("    --track-count               Set the track count in tag information.\n");
    printf("    --disc                      Set the disc number in tag information.\n");
    printf("    --rate                      Set the rate in ptag information.\n");
    printf("    --picture                   Set the picture such as cover, album art, thumbnail in tag information.\n");
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

    print("Encode completed.");

    sprintf_s(buffer, buffer_size, "Output: %s\0", output_file_path);
    print(buffer);

    print_return();

    sprintf_s(buffer, buffer_size, "[RESULT]\0");
    print(buffer);

    sprintf_s(buffer, buffer_size, "Elapsed time: %d msec.\0", encode_end - encode_start);
    print(buffer);

    src_file_size = get_file_size(input_file_path);
    dst_file_size = get_file_size(output_file_path);

    sprintf_s(buffer, buffer_size, "Source file size: %.2f MiB\0", (src_file_size / 1048576.0));
    print(buffer);

    sprintf_s(buffer, buffer_size, "Destination file size: %.2f MiB\0", (dst_file_size / 1048576.0));
    print(buffer);

    sprintf_s(buffer, buffer_size, "Compression rate: %.2f%% of the original file size.\0", ((double)dst_file_size / src_file_size) * 100.0);
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

    sprintf_s(buffer, buffer_size, "Sample rate:       %d Hz\0", decoder->sample_rate);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Bits per sample:   %d Bits\0", decoder->bits_per_sample);
    print(buffer);

    sprintf_s(buffer, buffer_size, "channels:          %d Channels\0", decoder->num_channels);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Mid-Side stereo:   %s\0", decoder->use_mid_side_stereo ? "Yes" : "No");
    print(buffer);

    sprintf_s(buffer, buffer_size, "SSLMS filter taps: %d\0", decoder->filter_taps);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Block size:        %d Samples\0", decoder->block_size);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Total samples:     %d Samples\0", decoder->num_total_samples);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Total blocks:      %d Blocks\0", decoder->num_blocks);
    print(buffer);

    free(buffer);
}

/*!
 * @brief                   Print tag informations.
 * @param *decoder          Pointer to tag.
 */
static void print_tag(tag* ptag) {
    size_t buffer_size = sizeof(char) * 1024;
    char* buffer = (char*)malloc(buffer_size);

    if (buffer == NULL) {
        return;
    }

    if (ptag == NULL) {
        free(buffer);
        return;
    }

    sprintf_s(buffer, buffer_size, "[TAG INFORMATION]\0");
    print(buffer);

    sprintf_s(buffer, buffer_size, "Title:        %s\0", ptag->title);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Album:        %s\0", ptag->album);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Artist:       %s\0", ptag->artist);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Album-artist: %s\0", ptag->album_artist);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Subtitle:     %s\0", ptag->subtitle);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Publisher:    %s\0", ptag->publisher);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Composer:     %s\0", ptag->composer);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Songwriter:   %s\0", ptag->songwriter);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Conductor:    %s\0", ptag->conductor);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Copyright:    %s\0", ptag->copyright);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Comment:      %s\0", ptag->comment);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Genre:        %s\0", ptag->genre);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Year:         %d\0", ptag->year);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Track number: %d\0", ptag->track_number);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Track count:  %d\0", ptag->track_count);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Disc number:  %d\0", ptag->disc);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Rating:       %d\0", ptag->rate);
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

    print("Decode completed.");
    print_return();

    sprintf_s(buffer, buffer_size, "[RESULT]\0");
    print(buffer);

    sprintf_s(buffer, buffer_size, "Output: %s\0", output_file_path);
    print(buffer);

    sprintf_s(buffer, buffer_size, "Elapsed time: %d msec.\0", decode_end - decode_start);
    print(buffer);

    print_return();
    
    /* フォーマット情報を出力 */
    print_format_info(decoder);

    /* タグ情報を出力 */
    print_tag(decoder->tag);
    
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
    tag* ptag;

    /* Delete old file. */
    remove(output_file_path);

    /* Create instance of WAV file reader. */
    reader = wave_file_reader_create(input_file_path);

    /* Get the number of samples from WAV file. */
    n = wave_file_reader_get_num_samples(reader);

    if (has_tag) {
        ptag = (tag*)malloc(sizeof(tag));
        tag_init(ptag);
        if (tag_title != NULL) ptag->title = tag_title;
        if (tag_album != NULL) ptag->album = tag_album;
        if (tag_artist != NULL) ptag->artist = tag_artist;
        if (tag_album_artist != NULL) ptag->album_artist = tag_album_artist;
        if (tag_subtitle != NULL) ptag->subtitle = tag_subtitle;
        if (tag_publisher != NULL) ptag->publisher = tag_publisher;
        if (tag_composer != NULL) ptag->composer = tag_composer;
        if (tag_songwriter != NULL) ptag->songwriter = tag_songwriter;
        if (tag_conductor != NULL) ptag->conductor = tag_conductor;
        if (tag_copyright != NULL) ptag->copyright = tag_copyright;
        if (tag_genre != NULL) ptag->genre = tag_genre;
        if (tag_comment != NULL) ptag->comment = tag_comment;
        ptag->year = tag_year;
        ptag->track_number = tag_track_number;
        ptag->track_count = tag_track_count;
        ptag->disc = tag_disc;
        ptag->rate = tag_rate;

        if (tag_picture_path != NULL) {
            tag_set_picture_from_path(ptag, tag_picture_path);
        }
    }
    else {
        ptag = NULL;
    }

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
        ptag);

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
    wave_file_writer_set_pcm_format(writer, decoder->sample_rate, decoder->bits_per_sample, decoder->num_channels);
    wave_file_writer_set_num_samples(writer, decoder->num_total_samples);

    /* Save start time */
    start = clock();
    wave_file_writer_begin_write(writer);

    /* Decode and write all samples. */
    for (i = 0; i < decoder->num_total_samples; ++i) {
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
    else{
        print("[Fatal Error] No input file.");
    }

    free(input_file_path);
    free(output_file_path);
}