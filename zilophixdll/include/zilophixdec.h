#ifndef ZILOPHIXDEC_HEADER_INCLUDED
#define ZILOPHIXDEC_HEADER_INCLUDED

#include "decoder.h"
#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>

typedef decoder *HDECODER;
typedef uint8_t* BYTEPTR;

/*!
* @brief            �w�肳�ꂽ�p�X�̃t�@�C�����f�R�[�h����f�R�[�_�̃n���h����Ԃ��܂��B
* @param path       �J���t�@�C���̃p�X
* @return           �f�R�[�_�̃n���h��
*/
HDECODER __declspec(dllexport) CreateDecoderA(LPCSTR path);

/*!
* @brief            �w�肳�ꂽ�p�X�̃t�@�C�����f�R�[�h����f�R�[�_�̃n���h����Ԃ��܂��B
* @param path       �J���t�@�C���̃p�X
* @return           �f�R�[�_�̃n���h��
*/
HDECODER __declspec(dllexport) CreateDecoderW(LPCWSTR path);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_��������܂��B
* @param decoder    �f�R�[�_�̃n���h��
*/
void __declspec(dllexport) FreeDecoder(HDECODER decoder);


/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C������܂��B
* @param decoder    �f�R�[�_�̃n���h��
*/
void __declspec(dllexport) DecoderCloseFile(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̃T���v�����O���g�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �T���v�����O���g��
*/
uint32_t __declspec(dllexport) DecoderGetSampleRate(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̃`�����l�������擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �`�����l����
*/
uint32_t __declspec(dllexport) DecoderGetChannels(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̗ʎq���r�b�g�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
*/
uint32_t __declspec(dllexport) DecoderGetBitsPerSample(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̃T���v�������擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �T���v����
*/
uint32_t __declspec(dllexport) DecoderGetNumTotalSamples(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̃u���b�N�̃T�C�Y���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �u���b�N�̃T�C�Y
*/
uint16_t __declspec(dllexport) DecoderGetBlockSize(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̃u���b�N�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �u���b�N��
*/
uint32_t __declspec(dllexport) DecoderGetNumBlocks(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���ŁA�~�b�h�T�C�h�X�e���I���g�p����Ă��邩�ǂ����������t���O���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �~�b�h�T�C�h�X�e���I���g�p����Ă����true���A�g�p����Ă��Ȃ����false��Ԃ��܂��B
*/
bool __declspec(dllexport) DecoderGetUseMidSideStereo(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C������1�T���v���ǂݍ��݂܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �ǂݍ��񂾃T���v���B�߂�l�̌^��int32_t�ł����A���ۂ̃r�b�g���͗ʎq���r�b�g���Ɠ����ƂȂ�܂��B
*/
int32_t __declspec(dllexport) DecoderReadSample(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���ŁA�f�R�[�h����T���v���̃I�t�Z�b�g��ݒ肵�܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @param offset     �T���v���̃I�t�Z�b�g
*/
void __declspec(dllexport) DecoderSetSampleOffset(HDECODER decoder, uint32_t offset);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���́A�f�R�[�h���̃T���v���̃I�t�Z�b�g���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �T���v���̃I�t�Z�b�g
*/
uint32_t __declspec(dllexport) DecoderGetSampleOffset(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C�����A�~���b�P�ʂŎw�肳�ꂽ���ԂɃV�[�N���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @param msec       �V�[�N�掞�ԁi�~���b�P�ʁj
*/
void __declspec(dllexport) DecoderSeekTo(HDECODER decoder, uint32_t msec);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���̉��t���Ԃ��~���b�P�ʂŎ擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �f�R�[�_�ŊJ����Ă���t�@�C���̉��t���ԁi�~���b�P�ʁj
*/
uint32_t __declspec(dllexport) DecoderGetDurationMsec(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���ɁA�^�O��񂪊܂܂�Ă��邩�ǂ����𔻒肵�܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O��񂪊܂܂�Ă����true���A�܂܂�Ă��Ȃ����false��Ԃ��܂��B
*/
bool __declspec(dllexport) DecoderContainsTagInfo(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�^�C�g�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�^�C�g����Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetTitle(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�A���o�������擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�A���o������Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetAlbum(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�A�[�e�B�X�g�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�A�[�e�B�X�g����Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetArtist(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�A���o���A�[�e�B�X�g���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�A���o���A�[�e�B�X�g��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetAlbumArtist(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�T�u�^�C�g�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�T�u�^�C�g����Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetSubtitle(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A���s�҂��擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ���s�҂�Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetPublisher(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A��Ȏ҂��擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ��Ȏ҂�Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetComposer(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�쎌�҂��擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�쎌�҂�Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetSongwriter(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�w���҂��擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�w���҂�Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetConductor(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A���쌠�\�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ���쌠�\����Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetCopyright(HDECODER decoder);

/*!
* @brief            �f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�W�����������擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�W����������Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetGenre(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�R�����g���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�R�����g��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A\0��Ԃ��܂��B
*/
LPCSTR __declspec(dllexport) DecoderGetComment(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�N���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�N��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
uint16_t __declspec(dllexport) DecoderGetYear(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�g���b�N�ԍ����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�g���b�N�ԍ���Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
uint16_t __declspec(dllexport) DecoderGetTrackNumber(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�g���b�N�����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�g���b�N����Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
uint16_t __declspec(dllexport) DecoderGetTrackCount(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�f�B�X�N�ԍ����擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�f�B�X�N�ԍ���Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
uint16_t __declspec(dllexport) DecoderGetDisc(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A���[�e�B���O���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ���[�e�B���O��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
uint16_t __declspec(dllexport) DecoderGetRating(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�J�o�[�摜�̃T�C�Y���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�J�o�[�摜�f�[�^�̃T�C�Y��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
uint32_t __declspec(dllexport) DecoderGetPictureSize(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�J�o�[�摜�̃t�H�[�}�b�g�R�[�h���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�J�o�[�摜�f�[�^�̃t�H�[�}�b�g�R�[�h��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�A0��Ԃ��܂��B
*/
unsigned char __declspec(dllexport) DecoderGetPictureFormatCode(HDECODER decoder);

/*!
* @brief            �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ����Ă���t�@�C���Ɋ܂܂��^�O��񂩂�A�J�o�[�摜���擾���܂��B
* @param decoder    �f�R�[�_�̃n���h��
* @return           �^�O�ɐݒ肳�ꂽ�J�o�[�摜�f�[�^���i�[���ꂽ�̈�ւ̃|�C���^��Ԃ��܂��B�^�O���܂܂�Ă��Ȃ��ꍇ��A�ݒ肳��Ă��Ȃ��ꍇ�ANULL��Ԃ��܂��B
*/
BYTEPTR __declspec(dllexport) DecoderGetPicture(HDECODER decoder);

void __declspec(dllexport) DecodeFileA(LPCSTR input, LPCSTR output);

#endif