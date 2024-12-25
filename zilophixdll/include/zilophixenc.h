#ifndef ZILOPHIXENC_HEADER_INCLUDED
#define ZILOPHIXENC_HEADER_INCLUDED

#include "encoder.h"
#include <stdint.h>
#include <stdbool.h>
#include <Windows.h>

typedef encoder* HENCODER;

/*!
 * @brief                           �w�肳�ꂽ�ݒ�ŁA�G���R�[�_�̃n���h���𐶐����܂��B
 * @param output                    �o�͐�̃t�@�C���̃p�X
 * @param sample_rate               �T���v�����O���g��
 * @param bits_per_sample           �ʎq���r�b�g��
 * @param num_channels              �`�����l����
 * @param num_samples               ���v�T���v����
 * @param block_size                �u���b�N�T�C�Y
 * @param use_mid_side_stereo       �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O
 * @param filter_taps               LMS�t�B���^�̃^�b�v��
 * @param *tag                      �^�O���
 */
HENCODER __declspec(dllexport) CreateEncoderFromPathA(
    LPCSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag);

/*!
 * @brief                           �w�肳�ꂽ�ݒ�ŁA�G���R�[�_�̃n���h���𐶐����܂��B
 * @param output                    �o�͐�̃t�@�C���̃p�X
 * @param sample_rate               �T���v�����O���g��
 * @param bits_per_sample           �ʎq���r�b�g��
 * @param num_channels              �`�����l����
 * @param num_samples               ���v�T���v����
 * @param block_size                �u���b�N�T�C�Y
 * @param use_mid_side_stereo       �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O
 * @param filter_taps               LMS�t�B���^�̃^�b�v��
 * @param *tag                      �^�O���
 */
HENCODER __declspec(dllexport) CreateEncoderFromPathW(
    LPCWSTR output,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag);

/*!
 * @brief                           �w�肳�ꂽ�ݒ�ŁA�G���R�[�_�̃n���h���𐶐����܂��B
 * @param *file                     �o�͐�̃t�@�C���n���h��
 * @param sample_rate               �T���v�����O���g��
 * @param bits_per_sample           �ʎq���r�b�g��
 * @param num_channels              �`�����l����
 * @param num_samples               ���v�T���v����
 * @param block_size                �u���b�N�T�C�Y
 * @param use_mid_side_stereo       �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O
 * @param filter_taps               LMS�t�B���^�̃^�b�v��
 * @param *tag                      �^�O���
 */
HENCODER __declspec(dllexport) CreateEncoderFromFile(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag);

/*!
 * @brief           �G���R�[�_��������܂��B
 * @param encoder   �G���R�[�_�̃n���h��
 */
void __declspec(dllexport) FreeEncoder(HENCODER encoder);

/*!
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�ŁA�w�肳�ꂽ�T���v�����G���R�[�h���܂��B
 * @param encoder   �G���R�[�_�̃n���h��
 * @param sample    �T���v��
 */
void __declspec(dllexport) EncoderWriteSample(HENCODER encoder, int sample);

/*!
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�ł̃G���R�[�h�̏I���������s���܂��B���ׂẴT���v���̃G���R�[�h���I��������ɁA�K���Ăяo���Ă��������B
 * @param encoder   �G���R�[�_�̃n���h��
 */
void __declspec(dllexport) EncoderEndWrite(HENCODER encoder);

void __declspec(dllexport) EncodeFileA(
    LPCSTR input,
    LPCSTR output,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps);

#endif
