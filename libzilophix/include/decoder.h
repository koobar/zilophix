#ifndef DECODER_HEADER_INCLUDED
#define DECODER_HEADER_INCLUDED

#include "bit_stream.h"
#include "lms.h"
#include "block.h"
#include "code.h"
#include "tag.h"
#include "polynomial_predictor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*!
 * @brief �f�R�[�_
 */
typedef struct {
    FILE* file;                                     /* �t�@�C���n���h�� */
    bit_stream* bit_stream;                         /* �r�b�g�X�g���[���̃n���h�� */

    uint8_t format_version;                         /* �t�H�[�}�b�g�̃o�[�W���� */
    uint32_t sample_rate;                           /* �T���v�����O���g�� */
    uint8_t bits_per_sample;                        /* PCM�̗ʎq���r�b�g�� */
    uint8_t num_channels;                           /* �`�����l���� */
    uint32_t num_total_samples;                     /* �t�@�C���Ɋ܂܂��T���v���̑��� */
    uint8_t filter_taps;                            /* SSLMS�t�B���^�̃^�b�v�� */
    uint16_t block_size;                            /* �u���b�N�i�����ɂ̓T�u�u���b�N�j�Ɋ܂܂��T���v����*/
    bool use_mid_side_stereo;                       /* �~�b�h�T�C�h�X�e���I���g�p����Ă��邩�ǂ����������t���O */
    uint32_t num_blocks;                            /* �t�@�C���Ɋ܂܂��u���b�N�̑��� */
    uint8_t reserved1;                              /* �\��ς�1 */
    uint8_t reserved2;                              /* �\��ς�2 */
    uint32_t audio_data_begin;
    uint32_t audio_data_size;

    lms** lms_filters;                              /* �`�����l������SSLMS�t�B���^�̃n���h�����i�[����̈� */
    polynomial_predictor** polynomial_predictors;   /* �`�����l�����̑������\����̃n���h�����i�[����̈� */

    tag* tag;                                       /* �^�O���̃n���h�� */
    code* coder;                                    /* �u���b�N�ǂݏ���API�̃n���h�� */
    block* current_block;                           /* �f�R�[�h���̃u���b�N�̃n���h�� */
    uint8_t current_read_sub_block_channel;         /* ���ɃT���v����ǂݍ��ޏꍇ�̃`�����l���̃I�t�Z�b�g */
    uint16_t current_read_sub_block_offset;         /* ���ɃT���v����ǂݍ��ޏꍇ�ɎQ�Ƃ���T�u�u���b�N�̃I�t�Z�b�g */

    uint32_t num_samples_read;                      /* �ǂݍ��ݍς݃T���v���� */
    bool is_seeking;                                /* �V�[�N�������ł��邩�ǂ����������t���O */
} decoder;

/*!
 * @brief                   �f�R�[�_�̃n���h���𐶐����܂��B
 * @param path              �f�R�[�h����t�@�C���̃n���h��
 * @return                  �f�R�[�_�̃n���h��
 */
decoder* decoder_create(FILE* file);

/*!
 * @brief                   �f�R�[�_��������܂��B
 * @param decoder           �f�R�[�_�̃n���h��
 */
void decoder_free(decoder* decoder);

/*!
 * @brief                   �t�@�C������܂��B
 * @param decoder           �f�R�[�_�̃n���h��
 */
void decoder_close(decoder* decoder);

/*!
 * @brief                   ����1�T���v����ǂݍ��݁APCM�T���v���Ƃ��ĕԂ��܂��B
 * @param decoder           �f�R�[�_�̃n���h��
 * @return                  �f�R�[�h���ꂽPCM�T���v��
 */
int32_t decoder_read_sample(decoder* decoder);

/*!
 * @brief                   �w�肳�ꂽ�I�t�Z�b�g�̃T���v���܂ŃV�[�N���܂��B
 * @param *decoder          �f�R�[�_�̃n���h��
 * @param sample_offset     �V�[�N��̃T���v���̃I�t�Z�b�g
 */
void decoder_seek_sample_to(decoder* decoder, uint32_t sample_offset);

/*!
 * @brief                   �~���b�P�ʂŎw�肳�ꂽ���Ԃ܂ŃV�[�N���܂��B
 * @param *decoder          �f�R�[�_�̃n���h��
 * @param sample_offset     �V�[�N�掞�ԁi�~���b�j
 */
void decoder_seek_milliseconds_to(decoder* decoder, uint32_t ms);

/*!
 * @brief                   �f�R�[�_���J���Ă���t�@�C���̉��t���Ԃ��~���b�P�ʂŎ擾���܂��B
 * @param *decoder          �f�R�[�_�̃n���h��
 * @return                  ���t���ԁi�~���b�P�ʁj
 */
uint32_t decoder_get_duration_ms(decoder* decoder);

#endif