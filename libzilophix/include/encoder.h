#ifndef ENCODER_HEADER_INCLUDED
#define ENCODER_HEADER_INCLUDED

#include "bit_stream.h"
#include "lms.h"
#include "block.h"
#include "code.h"
#include "tag.h"
#include "polynomial_predictor.h"
#include <stdbool.h>

/*!
 * @brief �G���R�[�_
 */
typedef struct {
    FILE* output_file;                                  /* �o�͐�t�@�C���̃n���h��*/
    bit_stream* output_bit_stream;                      /* �o�͐�r�b�g�X�g���[���̃n���h�� */
    uint32_t audio_data_size_position;
    uint32_t audio_data_begin_position;

    uint32_t sample_rate;                               /* �T���v�����O���g�� */
    uint8_t bits_per_sample;                            /* PCM�̗ʎq���r�b�g�� */
    uint8_t num_channels;                               /* �`�����l���� */
    uint32_t num_samples;                               /* �t�@�C���Ɋ܂܂��T���v���̑��� */

    uint8_t filter_taps;                                /* SSLMS�t�B���^�̃^�b�v�� */
    uint16_t block_size;                                /* �u���b�N�i�����ɂ̓T�u�u���b�N�j�Ɋi�[�����T���v���� */
    bool use_mid_side_stereo;                           /* �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O */
    uint32_t num_blocks;                                /* �t�@�C���Ɋ܂܂��u���b�N�� */
    uint8_t reserved1;                                  /* �\��ς�1 */
    uint8_t reserved2;                                  /* �\��ς�2 */
    uint32_t audio_data_position_position;

    lms** lms_filters;                                  /* �`�����l������SSLMS�t�B���^�̃n���h�����i�[�����̈� */
    polynomial_predictor** polynomial_predictors;       /* �`�����l�����̑������\����̃n���h�����i�[�����̈� */

    tag* tag;                                           /* �^�O���̃n���h�� */
    code* coder;                                        /* �u���b�N�ǂݏ���API�̃n���h�� */
    block* current_block;                               /* �G���R�[�h���̃u���b�N�̃n���h�� */
    uint8_t current_sub_block_channel;                  /* ���Ƀu���b�N�ɃT���v�����������ޏꍇ�̃`�����l���̃I�t�Z�b�g */
    uint16_t current_sub_block_offset;                  /* ���Ƀu���b�N�ɃT���v�����������ޏꍇ�̃T�u�u���b�N�̃I�t�Z�b�g */
} encoder;

/*!
 * @brief                           �w�肳�ꂽ�ݒ�ŁA�G���R�[�_�̃n���h���𐶐����܂��B
 * @param *file                     �o�͐�̃t�@�C���n���h��
 * @param sample_rate               �T���v�����O���g��
 * @param bits_per_sample           �ʎq���r�b�g��
 * @param num_channels              �`�����l����
 * @param num_samples               ���v�T���v����
 * @param block_size                �u���b�N�T�C�Y
 * @param use_mid_side_stereo       �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O
 * @param filter_taps               LMS�t�B���^�̍ő�^�b�v��
 * @param *tag                      �^�O���
 */
encoder* encoder_create(
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
 * @param *encoder  �G���R�[�_�̃n���h��
 */
void encoder_free(encoder* encoder);

/*!
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�ŁA�w�肳�ꂽ�T���v�����G���R�[�h���܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
 * @param sample    �T���v��
 */
void encoder_write_sample(encoder* encoder, int32_t sample);

/*!
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�ł̃G���R�[�h�̏I���������s���܂��B���ׂẴT���v���̃G���R�[�h���I��������ɁA�K���Ăяo���Ă��������B
 * @param *encoder  �G���R�[�_�̃n���h��
 */
void encoder_end_write(encoder* encoder);

#endif