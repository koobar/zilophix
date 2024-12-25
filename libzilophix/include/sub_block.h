#ifndef SUB_BLOCK_HEADER_INCLUDED
#define SUB_BLOCK_HEADER_INCLUDED

#include <stdint.h>

typedef struct {
    int32_t* samples;       /* �T���v���̈�̃|�C���^ */
    uint16_t size;          /* �T�u�u���b�N�Ɋi�[���ꂽ�T���v���� */
    uint8_t channel;        /* �T�u�u���b�N�ɑΉ�����`�����l�� */
} sub_block;

/*!
 * @brief               �T�u�u���b�N�����������܂��B
 * @param *sub_block    �T�u�u���b�N�̃n���h��
 * @param size          �T�u�u���b�N�̃T���v����
 * @param channel       �Ή�����`�����l��
 */
void sub_block_init(sub_block* sub_block, uint16_t size, uint8_t channel);

/*!
 * @brief               �T�u�u���b�N��������܂��B
 * @param *sub_block    �T�u�u���b�N�̃n���h��
 */
void sub_block_free(sub_block* sub_block);

#endif