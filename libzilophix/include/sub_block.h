#ifndef SUB_BLOCK_HEADER_INCLUDED
#define SUB_BLOCK_HEADER_INCLUDED

#include <stdint.h>

typedef struct {
    int32_t* samples;       /* サンプル領域のポインタ */
    uint16_t size;          /* サブブロックに格納されたサンプル数 */
    uint8_t channel;        /* サブブロックに対応するチャンネル */
} sub_block;

/*!
 * @brief               サブブロックを初期化します。
 * @param *sub_block    サブブロックのハンドル
 * @param size          サブブロックのサンプル数
 * @param channel       対応するチャンネル
 */
void sub_block_init(sub_block* sub_block, uint16_t size, uint8_t channel);

/*!
 * @brief               サブブロックを解放します。
 * @param *sub_block    サブブロックのハンドル
 */
void sub_block_free(sub_block* sub_block);

#endif