#ifndef BLOCK_HEADER_INCLUDED
#define BLOCK_HEADER_INCLUDED

#include "sub_block.h"
#include <stdint.h>

/*!
 * @brief ブロック
 */
typedef struct {
    sub_block** sub_blocks;
    uint16_t size;
    uint8_t num_channels;
} block;

/*!
 * @brief               ブロックを初期化します。
 * @param block         初期化するブロックのハンドル
 * @param size          ブロックに含まれるサブブロックのサンプル数 
 * @param num_channels  ブロックのチャンネル数（サブブロック数）
 */
void block_init(block* block, uint16_t size, uint8_t num_channels);

/*!
 * @brief       ブロックを解放します。
 * @param block ブロックのハンドル
 */
void block_free(block* block);

#endif