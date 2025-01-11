#ifndef CODE_HEADER_INCLUDED
#define CODE_HEADER_INCLUDED

#include "bit_stream.h"
#include "block.h"

typedef struct {
    bit_stream* bitstream;              /* Pointer of input bitstream */
    uint8_t bits_per_sample;
    uint8_t bits_of_entropy_parameter;
    uint8_t rice_parameter_max;
    uint8_t blank_partition_parameter;
    uint32_t* workA;                    /* Working memory A */
    uint32_t* workB;                    /* Working memory B */
} code;

/*!
 * @brief           Create new instance of code.
 * @param *stream   Pointer of bit_stream.
 * @return          Pointer of created instance.
 */
code* code_create(bit_stream* stream, uint8_t fmt_version, uint8_t bits_per_sample);

/*!
 * @brief           Release specified instance.
 * @param *coder    Pointer of code.
 */
void code_free(code* coder);

/*!
 * @brief           Write block to bitstream.
 * @param *coder    Pointer of code.
 * @param *block    Pointer of block.
 */
void code_write_block(code* coder, const block* block);

/*!
 * @brief           Read block from bitstream.
 * @param *coder    Pointer of code.
 * @param *block    Pointer of block.
 */
void code_read_block(code* coder, block* block);

#endif