#ifndef CODE_HEADER_INCLUDED
#define CODE_HEADER_INCLUDED

#include "bit_stream.h"
#include "block.h"

typedef struct {
    bit_stream* bitstream;              /* ���o�͗p�r�b�g�X�g���[���̃n���h�� */
    uint8_t bits_per_sample;
    uint8_t bits_of_entropy_parameter;
    uint8_t rice_parameter_max;
    uint8_t blank_partition_parameter;
    uint32_t* workA;                    /* ��Ɨ̈�A */
    uint32_t* workB;                    /* ��Ɨ̈�B */
} code;

/*!
 * @brief           �u���b�N��ǂݏ�������API�̃n���h���𐶐����܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @return          �u���b�N�ǂݏ���API�̃n���h��
 */
code* code_create(bit_stream* stream, uint8_t fmt_version, uint8_t bits_per_sample);

/*!
 * @brief           �u���b�N��ǂݏ�������API�̃n���h����������܂��B
 * @param *coder    �u���b�N�ǂݏ���API�̃n���h��
 */
void code_free(code* coder);

/*!
 * @brief           �u���b�N���������݂܂��B
 * @param *coder    �u���b�N�ǂݏ���API�̃n���h��
 * @param *block    �������ރu���b�N�̃n���h��
 */
void code_write_block(code* coder, const block* block);

/*!
 * @brief           �u���b�N��ǂݍ��݂܂��B
 * @param *coder    �u���b�N�ǂݏ���API�̃n���h��
 * @param *block    �ǂݍ��񂾃f�[�^���i�[����u���b�N�̃n���h��
 */
void code_read_block(code* coder, block* block);

#endif