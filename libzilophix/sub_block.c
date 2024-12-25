#include "./include/errcode.h"
#include "./include/sub_block.h"
#include <stdbool.h>
#include <stdlib.h>

/*!
 * @brief               サブブロックを初期化します。
 * @param *sub_block    サブブロックのハンドル
 * @param size          サブブロックのサンプル数
 * @param channel       対応するチャンネル
 */
void sub_block_init(sub_block* sub_block, uint16_t size, uint8_t channel) {
    sub_block->size = size;
    sub_block->channel = channel;
    sub_block->samples = (int32_t*)calloc(size, sizeof(int32_t));

    if (sub_block->samples == NULL) {
        report_error(ERROR_SUB_BLOCK_CANNOT_ALLOCATE_MEMORY);
    }
}

/*!
 * @brief               サブブロックを解放します。
 * @param *sub_block    サブブロックのハンドル
 */
void sub_block_free(sub_block* sub_block) {
    free(sub_block->samples);
}