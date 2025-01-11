#ifndef SUB_BLOCK_HEADER_INCLUDED
#define SUB_BLOCK_HEADER_INCLUDED

#include <stdint.h>

typedef struct {
    int32_t* samples;       /* The pointer of samples. */
    uint16_t size;          /* The number of samples. */
    uint8_t channel;        /* Channel index. */
} sub_block;

/*!
 * @brief               Initialize subblock.
 * @param *sub_block    Pointer of subblock.
 * @param size          The number of samples.
 * @param channel       Channel index.
 */
void sub_block_init(sub_block* sub_block, uint16_t size, uint8_t channel);

/*!
 * @brief               Release subblock.
 * @param *sub_block    Pointer of subblock.
 */
void sub_block_free(sub_block* sub_block);

#endif