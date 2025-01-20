#ifndef ZILOPHIXDEC_HEADER_INCLUDED
#define ZILOPHIXDEC_HEADER_INCLUDED

#include "decoder.h"
#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>

typedef decoder *HDECODER;
typedef uint8_t* BYTEPTR;

/*!
* @brief            Create new instance of ZilophiX decoder (ANSI).
* @param path       File name of ZilophiX file.
* @return           Pointer to a HDECODER structure.
*/
HDECODER __declspec(dllexport) CreateDecoderA(LPCSTR path);

/*!
* @brief            Create new instance of ZilophiX decoder (Unicode).
* @param path       File name of ZilophiX file.
* @return           Pointer to a HDECODER structure.
*/
HDECODER __declspec(dllexport) CreateDecoderW(LPCWSTR path);

/*!
* @brief            Release decider.
* @param decoder    Pointer to a HDECODER structure.
*/
void __declspec(dllexport) FreeDecoder(HDECODER decoder);


/*!
* @brief            Close file.
* @param decoder    Pointer to a HDECODER structure.
*/
void __declspec(dllexport) DecoderCloseFile(HDECODER decoder);

/*!
* @brief            Gets sample rate.
* @param decoder    Pointer to a HDECODER structure.
* @return           Sample rate.
*/
uint32_t __declspec(dllexport) DecoderGetSampleRate(HDECODER decoder);

/*!
* @brief            Gets the number of channels.
* @param decoder    Pointer to a HDECODER structure.
* @return           The number of channels.
*/
uint32_t __declspec(dllexport) DecoderGetChannels(HDECODER decoder);

/*!
* @brief            Gets bits per sample.
* @param decoder    Pointer to a HDECODER structure.
* @return           Bits per sample.
*/
uint32_t __declspec(dllexport) DecoderGetBitsPerSample(HDECODER decoder);

/*!
* @brief            Gets the number of samples.
* @param decoder    Pointer to a HDECODER structure.
* @return           The number of samples.
*/
uint32_t __declspec(dllexport) DecoderGetNumTotalSamples(HDECODER decoder);

/*!
* @brief            Gets block size.
* @param decoder    Pointer to a HDECODER structure.
* @return           Block size.
*/
uint16_t __declspec(dllexport) DecoderGetBlockSize(HDECODER decoder);

/*!
* @brief            Gets the number of blocks.
* @param decoder    Pointer to a HDECODER structure.
* @return           The number of blocks.
*/
uint32_t __declspec(dllexport) DecoderGetNumBlocks(HDECODER decoder);

/*!
* @brief            Gets the Mid-Side stereo flag.
* @param decoder    Pointer to a HDECODER structure.
* @return           Returns true if mid-side stereo is used, false if not.
*/
bool __declspec(dllexport) DecoderGetUseMidSideStereo(HDECODER decoder);

/*!
* @brief            Read sample as PCM.
* @param decoder    Pointer to a HDECODER structure.
* @return           Sample
*/
int32_t __declspec(dllexport) DecoderReadSample(HDECODER decoder);

/*!
* @brief            Sets the sample offset to decode.
* @param decoder    Pointer to a HDECODER structure.
* @param offset     Offset.
*/
void __declspec(dllexport) DecoderSetSampleOffset(HDECODER decoder, uint32_t offset);

/*!
* @brief            Gets the offset of the sample being decoded.
* @param decoder    Pointer to a HDECODER structure.
* @return           Offset.
*/
uint32_t __declspec(dllexport) DecoderGetSampleOffset(HDECODER decoder);

/*!
* @brief            Seeks to the specified time in milliseconds.
* @param decoder    Pointer to a HDECODER structure.
* @param msec       time in milliseconds.
*/
void __declspec(dllexport) DecoderSeekTo(HDECODER decoder, uint32_t msec);

/*!
* @brief            Gets the playback duration in milliseconds.
* @param decoder    Pointer to a HDECODER structure.
* @return           The playback duration in milliseconds.
*/
uint32_t __declspec(dllexport) DecoderGetDurationMsec(HDECODER decoder);

/*!
* @brief            Determines whether tag information is included.
* @param decoder    Pointer to a HDECODER structure.
* @return           Returns true if tag information is included, false if not.
*/
bool __declspec(dllexport) DecoderContainsTagInfo(HDECODER decoder);

/*!
* @brief            Gets the title from the tag information.
* @param decoder     Pointer to a HDECODER structure.
* @return           Title.
*/
LPCSTR __declspec(dllexport) DecoderGetTitle(HDECODER decoder);

/*!
* @brief            Gets the album name from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Album name.
*/
LPCSTR __declspec(dllexport) DecoderGetAlbum(HDECODER decoder);

/*!
* @brief            Gets the artist name from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Artist name.
*/
LPCSTR __declspec(dllexport) DecoderGetArtist(HDECODER decoder);

/*!
* @brief            Gets the album artist from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Album artists.
*/
LPCSTR __declspec(dllexport) DecoderGetAlbumArtist(HDECODER decoder);

/*!
* @brief            Gets the subtitle from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Subtitle.
*/
LPCSTR __declspec(dllexport) DecoderGetSubtitle(HDECODER decoder);

/*!
* @brief            Gets the publisher from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Publisher.
*/
LPCSTR __declspec(dllexport) DecoderGetPublisher(HDECODER decoder);

/*!
* @brief            Gets the composer from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Composer.
*/
LPCSTR __declspec(dllexport) DecoderGetComposer(HDECODER decoder);

/*!
* @brief            Gets the songwriter from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Songwriter.
*/
LPCSTR __declspec(dllexport) DecoderGetSongwriter(HDECODER decoder);

/*!
* @brief            Gets the conductor from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Conductor.
*/
LPCSTR __declspec(dllexport) DecoderGetConductor(HDECODER decoder);

/*!
* @brief            Gets the copyright from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Copyright.
*/
LPCSTR __declspec(dllexport) DecoderGetCopyright(HDECODER decoder);

/*!
* @brief            Gets the genre from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Genre
*/
LPCSTR __declspec(dllexport) DecoderGetGenre(HDECODER decoder);

/*!
* @brief            Gets the comment from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Comment
*/
LPCSTR __declspec(dllexport) DecoderGetComment(HDECODER decoder);

/*!
* @brief            Gets the year from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Year
*/
uint16_t __declspec(dllexport) DecoderGetYear(HDECODER decoder);

/*!
* @brief            Gets the track number from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Track number
*/
uint16_t __declspec(dllexport) DecoderGetTrackNumber(HDECODER decoder);

/*!
* @brief            Gets the track count from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Track count
*/
uint16_t __declspec(dllexport) DecoderGetTrackCount(HDECODER decoder);

/*!
* @brief            Gets the disc number from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Track count
*/
uint16_t __declspec(dllexport) DecoderGetDisc(HDECODER decoder);

/*!
* @brief            Gets the rating from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Rating.
*/
uint16_t __declspec(dllexport) DecoderGetRating(HDECODER decoder);

/*!
* @brief            Gets the picture size from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Picture size.
*/
uint32_t __declspec(dllexport) DecoderGetPictureSize(HDECODER decoder);

/*!
* @brief            Gets the picture format code from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           Picture format code.
*/
unsigned char __declspec(dllexport) DecoderGetPictureFormatCode(HDECODER decoder);

/*!
* @brief            Gets the picture from the tag information.
* @param decoder    Pointer to a HDECODER structure.
* @return           The pointer of picture binary data.
*/
BYTEPTR __declspec(dllexport) DecoderGetPicture(HDECODER decoder);

/*!
* @brief            Decode input file and save to output file as Linear PCM WAV format.
* @param input      Input file path.
* @param output     Output file path.
*/
void __declspec(dllexport) DecodeFileA(LPCSTR input, LPCSTR output);

/*!
* @brief            Decode input file and save to output file as Linear PCM WAV format.
* @param input      Input file path.
* @param output     Output file path.
*/
void __declspec(dllexport) DecodeFileW(LPCWSTR input, LPCWSTR output);

#endif