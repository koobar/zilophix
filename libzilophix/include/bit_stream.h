#ifndef BIT_STREAM_HEADER_INCLUDED
#define BIT_STREAM_HEADER_INCLUDED

#define BIT_STREAM_MODE_READ	0x00
#define BIT_STREAM_MODE_WRITE	0x01

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* file;
    uint32_t mode;
    uint32_t buffer;
    uint32_t buffer_position;
    uint32_t max_buffer_bytes;
    uint32_t max_buffer_bits;
} bit_stream;

/*!
 * @brief           Create new instance of bit_stream.
 * @param *file     File
 * @param mode      Mode
 * @return          A pointer to the created instance.
 */
bit_stream* bit_stream_create(FILE* file, uint32_t mode);

/*!
 * @brief           Initialize bit_stream instance.
 * @param *stream   Pointer of bit_stream.
 */
void bit_stream_init(bit_stream* stream);

/*!
 * @brief           Read single bit from bit_stream.
 * @param *stream   Pointer of bit_stream.
 * @return          Bit read.
 */
bool bit_stream_read_bit(bit_stream* stream);

/*!
 * @brief           Reads an unsigned integer number of bits from the bitstream.
 * @param *stream   Pointer of bit_stream.
 * @param bits      The number of integer bits to read
 * @return          Integer read.
 */
uint32_t bit_stream_read_uint(bit_stream* stream, uint32_t bits);

/*!
 * @brief           Reads a unary encoded integer from the bitstream.
 * @param *stream   Pointer of bit_stream.
 */
uint32_t bit_stream_read_unary(bit_stream* stream);

/*!
 * @brief           Reads a Rice-encoded signed integer from the bitstream.
 * @param *stream   Pointer of bit_stream.
 * @param bits      The number of integer bits to read.
 * @return          Integer read.
 */
uint32_t bit_stream_read_rice_signed(bit_stream* stream, uint32_t parameter);

/*!
 * @brief           Writes a single bit to the bitstream.
 * @param *stream   Pointer of bit_stream.
 * @param bit       Bit to write.
 */
void bit_stream_write_bit(bit_stream* stream, bool bit);

/*!
 * @brief           Writes an unsigned integer number of arbitrary bits to the bitstream.
 * @param *stream   Pointer of bit_stream.
 * @param value     Unsigned integer to write.
 * @param num_bits  The number of integer bits to write.
 */
void bit_stream_write_uint(bit_stream* stream, uint32_t value, uint32_t num_bits);

/*!
 * @brief           Write unsigned integer as unary code.
 * @param *stream   Pointer of bit_stream.
 * @param value     Unsigned integer to write.
 */
void bit_stream_write_unary(bit_stream* stream, uint32_t value);

/*!
 * @brief           Write signed integer as Rice code.
 * @param *stream   Pointer of bit_stream
 * @param value     Signed integer to write.
 * @param parameter Rice parameter.
 */
void bit_stream_write_signed_rice(bit_stream* stream, int32_t value, uint32_t parameter);

/*!
 * @brief			Close specified bit_stream.
 * @param *stream	Pointer of bit_stream.
 */
void bit_stream_close(bit_stream* stream);

#endif