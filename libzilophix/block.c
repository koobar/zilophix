#include "./include/block.h"
#include "./include/errcode.h"
#include <stdlib.h>

/*!
 * @brief               ブロックを初期化します。
 * @param block         初期化するブロックのハンドル
 * @param size          ブロックに含まれるサブブロックのサンプル数
 * @param num_channels  ブロックのチャンネル数（サブブロック数）
 */
void block_init(block* block, uint16_t size, uint8_t num_channels) {
    uint8_t ch;

    block->num_channels = num_channels;
    block->size = size;
    block->sub_blocks = (sub_block**)malloc(sizeof(sub_block*) * num_channels);

    if (block->sub_blocks == NULL) {
        report_error(ERROR_BLOCK_CANNOT_ALLOCATE_MEMORY);
        return;
    }

    for (ch = 0; ch < num_channels; ++ch) {
        block->sub_blocks[ch] = (sub_block*)malloc(sizeof(sub_block));
        sub_block_init(block->sub_blocks[ch], size, ch);
    }
}

/*!
 * @brief       ブロックを解放します。
 * @param block ブロックのハンドル
 */
void block_free(block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        sub_block_free(block->sub_blocks[ch]);
        free(block->sub_blocks[ch]);
    }

    free(block->sub_blocks);
    free(block);
}