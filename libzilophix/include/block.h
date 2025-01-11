#ifndef BLOCK_HEADER_INCLUDED
#define BLOCK_HEADER_INCLUDED

#include "sub_block.h"
#include <stdint.h>

typedef struct {
    sub_block** sub_blocks;
    uint16_t size;
    uint8_t num_channels;
} block;

/*!
 * @brief               Initialize block.
 * @param block         Pointer of block.
 * @param size          The number of samples.
 * @param num_channels  The number of channels.
 */
void block_init(block* block, uint16_t size, uint8_t num_channels);

/*!
 * @brief       Release specified block.
 * @param block Pointer of block.
 */
void block_free(block* block);

#endif