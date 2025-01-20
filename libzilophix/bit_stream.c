#include "./include/bit_stream.h"
#include "./include/macro.h"
#include "./include/errcode.h"
#include <stdlib.h>

static const uint32_t buffer_read_position_table[32] = {
    1, 2, 3, 4, 5, 6, 7, 8, 
    9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 0
};

static const uint32_t bit_mask_table[32] = {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,

    0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000
};

static void write_buffer(bit_stream* stream) {
    /* Write buffer to file. */
    fwrite(&stream->buffer, stream->max_buffer_bytes, 1, stream->file);
    stream->buffer_position = 0;
    stream->buffer = 0;
}

static void read_buffer(bit_stream* stream){
    /* Read binary data from file and store to buffer. */
    fread(&stream->buffer, stream->max_buffer_bytes, 1, stream->file);
}

/*!
 * @brief           Create new instance of bit_stream.
 * @param *file     File
 * @param mode      Mode
 * @return          A pointer to the created instance.
 */
bit_stream* bit_stream_create(FILE* file, uint32_t mode) {
    bit_stream* stream = (bit_stream*)malloc(sizeof(bit_stream));
    stream->mode = mode;
    stream->file = file;
    stream->buffer_position = 0;
    stream->buffer = 0;
    stream->max_buffer_bytes = sizeof(stream->buffer);
    stream->max_buffer_bits = stream->max_buffer_bytes * 8;

    return stream;
}

/*!
 * @brief           Initialize bit_stream instance.
 * @param *stream   Pointer to bit_stream structure.
 */
void bit_stream_init(bit_stream* stream) {
    stream->buffer = 0;
    stream->buffer_position = 0;
}

/*!
 * @brief           Read single bit from bit_stream.
 * @param *stream   Pointer to bit_stream structure.
 * @return          Bit read.
 */
bool bit_stream_read_bit(bit_stream* stream) {
    bool bit;

    if (stream->buffer_position == 0) {
        read_buffer(stream);
    }

    bit = (stream->buffer & bit_mask_table[stream->buffer_position]);
    stream->buffer_position = buffer_read_position_table[stream->buffer_position];

    return bit;
}

/*!
 * @brief           Reads an unsigned integer number of bits from the bitstream.
 * @param *stream   Pointer to bit_stream structure.
 * @param bits      The number of integer bits to read
 * @return          Integer read.
 */
uint32_t bit_stream_read_uint(bit_stream* stream, uint32_t bits) {
    uint32_t value = 0;
    uint32_t i;
    
    for (i = 0; i != bits; ++i) {
        value |= (bit_stream_read_bit(stream) << (bits - 1 - i));
    }
    
    return value;
}

/*!
 * @brief           Reads a unary encoded integer from the bitstream.
 * @param *stream   Pointer to bit_stream structure.
 */
uint32_t bit_stream_read_unary(bit_stream* stream){
    uint32_t result = 0;

    while (bit_stream_read_bit(stream)) {
        ++result;
    }

    return result;
}

/*!
 * @brief           Reads a Rice-encoded signed integer from the bitstream.
 * @param *stream   Pointer to bit_stream structure.
 * @param bits      The number of integer bits to read.
 * @return          Integer read.
 */
uint32_t bit_stream_read_rice_signed(bit_stream* stream, uint32_t parameter){
    uint32_t quotient;
    uint32_t remainder;

    /* Reads the unary encoded quotient. */
    quotient = bit_stream_read_unary(stream);

    /* Read the remainder. */
    remainder = bit_stream_read_uint(stream, parameter);

    /* Decode value and return it. */
    return CONVERT_UINT32_TO_INT32(LSHIFT(quotient, parameter) | remainder);
}

/*!
 * @brief           Writes a single bit to the bitstream.
 * @param *stream   Pointer to bit_stream structure.
 * @param bit       Bit to write.
 */
void bit_stream_write_bit(bit_stream* stream, bool bit) {
    if (stream->buffer_position == stream->max_buffer_bits) {
        write_buffer(stream);
    }

    stream->buffer |= (bit << stream->buffer_position);
    stream->buffer_position++;
}

/*!
 * @brief           Writes an unsigned integer number of arbitrary bits to the bitstream.
 * @param *stream   Pointer to bit_stream structure.
 * @param value     Unsigned integer to write.
 * @param num_bits  The number of integer bits to write.
 */
void bit_stream_write_uint(bit_stream* stream, uint32_t value, uint32_t num_bits) {
    int32_t i;

    for (i = num_bits - 1; i >= 0; --i) {
        bit_stream_write_bit(stream, (value >> i) & 1);
    }
}

/*!
 * @brief           Write unsigned integer as unary code.
 * @param *stream   Pointer to bit_stream structure.
 * @param value     Unsigned integer to write.
 */
void bit_stream_write_unary(bit_stream* stream, uint32_t value){
    uint32_t i;

    for (i = 0; i < value; ++i) {
        bit_stream_write_bit(stream, 1);
    }
    bit_stream_write_bit(stream, 0);
}

/*!
 * @brief           Write signed integer as Rice code.
 * @param *stream   Pointer to bit_stream structure
 * @param value     Signed integer to write.
 * @param parameter Rice parameter.
 */
void bit_stream_write_signed_rice(bit_stream* stream, int32_t value, uint32_t parameter){
    uint32_t val;
    uint32_t quotient;
    uint32_t remainder;

    val = CONVERT_INT32_TO_UINT32(value);
    quotient = RSHIFT(val, parameter);
    remainder = val - LSHIFT(quotient, parameter);

    /* Write quotient as unary code. */
    bit_stream_write_unary(stream, quotient);

    /* Write remainder. */
    bit_stream_write_uint(stream, remainder, parameter);
}

/*!
 * @brief			Close specified bit_stream.
 * @param *stream	Pointer to bit_stream structure.
 */
void bit_stream_close(bit_stream* stream) {
    if (stream->mode == BIT_STREAM_MODE_WRITE) {
        write_buffer(stream);
    }

    stream->buffer_position = 0;
    stream->buffer = 0;
}