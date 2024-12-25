#ifndef BLOCK_HEADER_INCLUDED
#define BLOCK_HEADER_INCLUDED

#include "sub_block.h"
#include <stdint.h>

/*!
 * @brief �u���b�N
 */
typedef struct {
    sub_block** sub_blocks;
    uint16_t size;
    uint8_t num_channels;
} block;

/*!
 * @brief               �u���b�N�����������܂��B
 * @param block         ����������u���b�N�̃n���h��
 * @param size          �u���b�N�Ɋ܂܂��T�u�u���b�N�̃T���v���� 
 * @param num_channels  �u���b�N�̃`�����l�����i�T�u�u���b�N���j
 */
void block_init(block* block, uint16_t size, uint8_t num_channels);

/*!
 * @brief       �u���b�N��������܂��B
 * @param block �u���b�N�̃n���h��
 */
void block_free(block* block);

#endif