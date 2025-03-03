#ifndef ZILOPHIXENC_HEADER_INCLUDED
#define ZILOPHIXENC_HEADER_INCLUDED

#include "encoder.h"
#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>

typedef encoder ENCODER;
typedef ENCODER* HENCODER;

/*!
 * @brief                           Create new instance of encoder (ANSI).
 * @param output                    Output file path (ANSI string).
 * @param sample_rate               Sample rate.
 * @param bits_per_sample           PCM Quantization bits.
 * @param num_channels              The number of channels.
 * @param num_samples               The number of sapmles.
 * @param block_size                Block size.
 * @param use_mid_side_stereo       Mid-Side stereo flag.
 * @param filter_taps               The taps of SSLMS filter.
 * @param output_format_version     Output format version.
 * @return                          Pointer to HENCODER.
 */
HENCODER __declspec(dllexport) ZpXCreateEncoderFromPathA(
    LPCSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    uint8_t output_format_version);

/*!
 * @brief                           Create new instance of encoder.
 * @param output                    Output file path (Unicode string).
 * @param sample_rate               Sample rate
 * @param bits_per_sample           PCM Quantization bits.
 * @param num_channels              The number of channels.
 * @param num_samples               The number of samples.
 * @param block_size                Block size.
 * @param use_mid_side_stereo       Mid-Side stereo flag.
 * @param filter_taps               The taps of SSLMS filter.
 * @param output_format_version     Output format version.
 * @return                          Pointer to HENCODER.
 */
HENCODER __declspec(dllexport) ZpXCreateEncoderFromPathW(
    LPCWSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    uint8_t output_format_version);

/*!
 * @brief                           Create new instance of encoder from file.
 * @param *file                     File
 * @param sample_rate               Sample rate.
 * @param bits_per_sample           PCM quantization bits.
 * @param num_channels              The number of channels.
 * @param num_samples               The number of samples.
 * @param block_size                Block size.
 * @param use_mid_side_stereo       Mid-Side stereo flag.
 * @param filter_taps               The taps of SSLMS filter.
 * @param output_format_version     Output format version.
 * @return                          Pointer to HENCODER.
 */
HENCODER __declspec(dllexport) ZpXCreateEncoderFromFile(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    uint8_t output_format_version);

/*!
 * @brief                           Release encoder.
 * @param encoder                   Pointer to HENCODER.
 */
void __declspec(dllexport) ZpXFreeEncoder(HENCODER encoder);

/*!
 * @brief                           Encode and write sample.
 * @param encoder                   Pointer to HENCODER.
 * @param sample                    Sample
 */
void __declspec(dllexport) ZpXWriteSample(HENCODER encoder, int sample);

/*!
 * @brief                           Finalizes the encoding process. Be sure to call this function after all samples have been encoded.
 * @param encoder                   Pointer to HENCODER.
 */
void __declspec(dllexport) ZpXEndWrite(HENCODER encoder);

/*!
* @brief                            Encode WAV input file and save to output file.
* @param input                      Input file path.
* @param output                     Output file path.
* @param output_format_version      Output format version.
*/
void __declspec(dllexport) ZpXEncodeFileA(
    LPCSTR input,
    LPCSTR output,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    uint8_t output_format_version);

/*!
* @brief                            Encode WAV input file and save to output file.
* @param input                      Input file path.
* @param output                     Output file path.
* @param output_format_version      Output format version.
*/
void __declspec(dllexport) ZpXEncodeFileW(
    LPCWSTR input,
    LPCWSTR output,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    uint8_t output_format_version);

#endif
