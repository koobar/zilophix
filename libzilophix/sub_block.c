#include "./include/errcode.h"
#include "./include/sub_block.h"
#include <stdbool.h>
#include <stdlib.h>

/*!
 * @brief               Initialize subblock.
 * @param *sub_block    Pointer to subblock.
 * @param size          The number of samples.
 * @param channel       Channel index.
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
 * @brief               Release subblock.
 * @param *sub_block    Pointer to subblock.
 */
void sub_block_free(sub_block* sub_block) {
    free(sub_block->samples);
}