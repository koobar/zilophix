#ifndef ZILOPHIX_HEADER_INCLUDED
#define ZILOPHIX_HEADER_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define LIBZILOPHIX_VERSION_MAJOR               1                           /* Library major version */
#define LIBZILOPHIX_VERSION_MINOR               2                           /* Library minor version  */

#define FORMAT_VERSION_1_0                      0x10                        /* Version code of ZilophiX 1.0 */
#define FORMAT_VERSION_1_1                      0x11                        /* Version code of ZilophiX 1.1 */
#define FORMAT_VERSION_1_2                      0x12                        /* Version code of ZilophiX 1.2 */

#define FORMAT_VERSION_CURRENT                  FORMAT_VERSION_1_2          /* The version code of the encoder output data. */
#define CURRENT_FORMAT_VERSION_NAME             "1.2"                       /* Current format version name. */
#define ZILOPHIX_DECODER_FORMAT_VERSION         FORMAT_VERSION_1_0          /* The version code of the decoder that can decode the code. */

#define SIZE_OF_MAGIC_NUMBER                    3
#define HEADER_OFFSET_OF_AUDIO_DATA_OFFSET      SIZE_OF_MAGIC_NUMBER + offsetof(zilophix_header, audio_data_offset)
#define HEADER_OFFSET_OF_AUDIO_DATA_SIZE        SIZE_OF_MAGIC_NUMBER + offsetof(zilophix_header, audio_data_size)

typedef struct {
    uint8_t format_version;                             /* Format version. */
    uint32_t sample_rate;                               /* Sample rate */
    uint8_t bits_per_sample;                            /* PCM quantization bits. */
    uint8_t num_channels;                               /* The number of channels. */
    uint32_t num_samples;                               /* The number of samples. */
    uint8_t filter_taps;                                /* The number of SSLMS filter taps. */
    uint16_t block_size;                                /* Block size. */
    bool use_mid_side_stereo;                           /* Mid-Side stereo flag. */
    uint32_t num_blocks;                                /* The number of blocks. */
    uint8_t reserved1;                                  /* Reserved. */
    uint8_t reserved2;                                  /* Reserved. */

    uint32_t id_audio_data_offset;
    uint32_t audio_data_offset;                         /* The offset of audio data. */
    uint32_t id_audio_data_size;
    uint32_t audio_data_size;                           /* The size of audio data. */
} zilophix_header;

void zilophix_header_init(zilophix_header* header);

#endif