#ifndef ZILOPHIXDEC_HEADER_INCLUDED
#define ZILOPHIXDEC_HEADER_INCLUDED

#include "decoder.h"
#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>

typedef decoder DECODER;
typedef DECODER* HDECODER;

/*!
* @brief            Create new instance of ZilophiX decoder (ANSI).
* @param path       File name of ZilophiX file.
* @return           Pointer to a HDECODER structure.
*/
HDECODER __declspec(dllexport) ZpXCreateDecoderA(LPCSTR path);

/*!
* @brief            Create new instance of ZilophiX decoder (Unicode).
* @param path       File name of ZilophiX file.
* @return           Pointer to a HDECODER structure.
*/
HDECODER __declspec(dllexport) ZpXCreateDecoderW(LPCWSTR path);

/*!
* @brief            Release decider.
* @param decoder    Pointer to a HDECODER structure.
*/
void __declspec(dllexport) ZpXFreeDecoder(HDECODER decoder);


/*!
* @brief            Close file.
* @param decoder    Pointer to a HDECODER structure.
*/
void __declspec(dllexport) ZpXCloseFile(HDECODER decoder);

/*!
* @brief            Gets sample rate.
* @param decoder    Pointer to a HDECODER structure.
* @return           Sample rate.
*/
uint32_t __declspec(dllexport) ZpXGetSampleRate(HDECODER decoder);

/*!
* @brief            Gets the number of channels.
* @param decoder    Pointer to a HDECODER structure.
* @return           The number of channels.
*/
uint32_t __declspec(dllexport) ZpXGetChannels(HDECODER decoder);

/*!
* @brief            Gets bits per sample.
* @param decoder    Pointer to a HDECODER structure.
* @return           Bits per sample.
*/
uint32_t __declspec(dllexport) ZpXGetBitsPerSample(HDECODER decoder);

/*!
* @brief            Gets the number of samples.
* @param decoder    Pointer to a HDECODER structure.
* @return           The number of samples.
*/
uint32_t __declspec(dllexport) ZpXGetNumTotalSamples(HDECODER decoder);

/*!
* @brief            Gets block size.
* @param decoder    Pointer to a HDECODER structure.
* @return           Block size.
*/
uint16_t __declspec(dllexport) ZpXGetBlockSize(HDECODER decoder);

/*!
* @brief            Gets the number of blocks.
* @param decoder    Pointer to a HDECODER structure.
* @return           The number of blocks.
*/
uint32_t __declspec(dllexport) ZpXGetNumBlocks(HDECODER decoder);

/*!
* @brief            Gets the Mid-Side stereo flag.
* @param decoder    Pointer to a HDECODER structure.
* @return           Returns true if mid-side stereo is used, false if not.
*/
bool __declspec(dllexport) ZpXGetUseMidSideStereo(HDECODER decoder);

/*!
* @brief            Read sample as PCM.
* @param decoder    Pointer to a HDECODER structure.
* @return           Sample
*/
int32_t __declspec(dllexport) ZpXReadSample(HDECODER decoder);

/*!
* @brief            Sets the sample offset to decode.
* @param decoder    Pointer to a HDECODER structure.
* @param offset     Offset.
*/
void __declspec(dllexport) ZpXSetSampleOffset(HDECODER decoder, uint32_t offset);

/*!
* @brief            Gets the offset of the sample being decoded.
* @param decoder    Pointer to a HDECODER structure.
* @return           Offset.
*/
uint32_t __declspec(dllexport) ZpXGetSampleOffset(HDECODER decoder);

/*!
* @brief            Seeks to the specified time in milliseconds.
* @param decoder    Pointer to a HDECODER structure.
* @param msec       time in milliseconds.
*/
void __declspec(dllexport) ZpXSeekTo(HDECODER decoder, uint32_t msec);

/*!
* @brief            Gets the playback duration in milliseconds.
* @param decoder    Pointer to a HDECODER structure.
* @return           The playback duration in milliseconds.
*/
uint32_t __declspec(dllexport) ZpXGetDurationMsec(HDECODER decoder);

/*!
* @brief            Decode input file and save to output file as Linear PCM WAV format.
* @param input      Input file path.
* @param output     Output file path.
*/
void __declspec(dllexport) ZpXDecodeFileA(LPCSTR input, LPCSTR output);

/*!
* @brief            Decode input file and save to output file as Linear PCM WAV format.
* @param input      Input file path.
* @param output     Output file path.
*/
void __declspec(dllexport) ZpXDecodeFileW(LPCWSTR input, LPCWSTR output);

#endif