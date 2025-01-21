#ifndef CODE_HEADER_INCLUDED
#define CODE_HEADER_INCLUDED

#include "bit_stream.h"
#include "block.h"

typedef struct {
    bit_stream* bitstream;                  /* Pointer to input bitstream */
    uint8_t bits_per_sample;                /* Bits per sample. */
    uint8_t minimum_partition_parameter;    /* Minimum partition parameter. */
    uint8_t maximum_partition_parameter;    /* Maximum partition parameter. */
    uint8_t bits_of_partition_parameter;    /* The bits of partition parameter need. */
    uint8_t maximum_partition_count;        /* Maximum partition count. */
    uint8_t bits_of_entropy_parameter;      /* The bits of entropy parameter need. */
    uint8_t blank_partition_parameter;      /* Blank partition parameter. */
    uint8_t rice_parameter_max;             /* Maximum rice parameter. */
    uint32_t* workA;                        /* Working memory A */
    uint32_t* workB;                        /* Working memory B */
} code;

/*!
 * @brief           Create new instance of code.
 * @param *stream   Pointer to bit_stream.
 * @return          Pointer to created instance.
 */
code* code_create(bit_stream* stream, uint8_t fmt_version, uint8_t bits_per_sample);

/*!
 * @brief           Release specified instance.
 * @param *coder    Pointer to code.
 */
void code_free(code* coder);

/*!
 * @brief           Write block to bitstream.
 * @param *coder    Pointer to code.
 * @param *block    Pointer to block.
 */
void code_write_block(code* coder, const block* block);

/*!
 * @brief           Read block from bitstream.
 * @param *coder    Pointer to code.
 * @param *block    Pointer to block.
 */
void code_read_block(code* coder, block* block);

#endif