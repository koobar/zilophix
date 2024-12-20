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
    /* バッファをファイルに書き込む */
    fwrite(&stream->buffer, stream->max_buffer_bytes, 1, stream->file);
    stream->buffer_position = 0;
    stream->buffer = 0;
}

static void read_buffer(bit_stream* stream){
    /* バッファサイズ分のデータをファイルから読み込む */
    fread(&stream->buffer, stream->max_buffer_bytes, 1, stream->file);
}

/*!
 * @brief           ビットストリームの領域を確保し、そのハンドルを返します。
 * @param *file     ビットストリームで扱うファイルのファイルハンドル
 * @param mode      ビットストリームのモード
 * @return          ビットストリームのハンドル
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
 * @brief           ビットストリームを初期化します。
 * @param *stream   ビットストリームのハンドル
 */
void bit_stream_init(bit_stream* stream) {
    stream->buffer = 0;
    stream->buffer_position = 0;
}

/*!
 * @brief           ビットストリームから1ビット読み込みます。
 * @param *stream   ビットストリームのハンドル
 * @return          読み込まれたビット
 */
bool bit_stream_read_bit(bit_stream* stream) {
    bool bit;

    if (stream->buffer_position == 0) {
        read_buffer(stream);
    }

    /* buffer_position が示す位置のビットを抽出 */
    bit = (stream->buffer & bit_mask_table[stream->buffer_position]);
    stream->buffer_position = buffer_read_position_table[stream->buffer_position];

    return bit;
}

/*!
 * @brief           ビットストリームから任意ビット数の整数を読み込みます。
 * @param *stream   ビットストリームのハンドル
 * @param bits      読み込む整数のビット数
 * @return          読み込まれた整数
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
 * @brief           ビットストリームからunary符号化された整数を読み込みます。
 * @param *stream   ビットストリームのハンドル
 */
uint32_t bit_stream_read_unary(bit_stream* stream){
    uint32_t result = 0;

    while (bit_stream_read_bit(stream)) {
        ++result;
    }

    return result;
}

/*!
 * @brief           ビットストリームからRice符号化された符号付き整数を読み込みます。
 * @param *stream   ビットストリームのハンドル
 * @param bits      読み込む整数のビット数
 * @return          読み込まれた整数
 */
uint32_t bit_stream_read_rice_signed(bit_stream* stream, uint32_t parameter){
    uint32_t quotient;
    uint32_t remainder;

    /* unary符号化された商を読み込む */
    quotient = bit_stream_read_unary(stream);

    /* 剰余を読み込む */
    remainder = bit_stream_read_uint(stream, parameter);

    /* 値を復元 */
    return CONVERT_UINT32_TO_INT32(LSHIFT(quotient, parameter) | remainder);
}

/*!
 * @brief           ビットストリームに1ビット書き込みます。
 * @param *stream   ビットストリームのハンドル
 * @param bit       書き込むビット
 */
void bit_stream_write_bit(bit_stream* stream, bool bit) {
    if (stream->buffer_position == stream->max_buffer_bits) {
        write_buffer(stream);
    }

    stream->buffer |= (bit << stream->buffer_position);
    stream->buffer_position++;
}

/*!
 * @brief           ビットストリームに任意ビット数の整数を書き込みます。
 * @param *stream   ビットストリームのハンドル
 * @param value     書き込む整数
 * @param num_bits  書き込む整数のビット数
 */
void bit_stream_write_uint(bit_stream* stream, uint32_t value, uint32_t num_bits) {
    int32_t i;

    for (i = num_bits - 1; i >= 0; --i) {
        bit_stream_write_bit(stream, (value >> i) & 1);
    }
}

/*!
 * @brief           ビットストリームに整数をunary符号として書き込みます。
 * @param *stream   ビットストリームのハンドル
 * @param value     書き込む整数
 */
void bit_stream_write_unary(bit_stream* stream, uint32_t value){
    uint32_t i;

    for (i = 0; i < value; ++i) {
        bit_stream_write_bit(stream, 1);
    }
    bit_stream_write_bit(stream, 0);
}

/*!
 * @brief           ビットストリームに整数をRice符号として書き込みます。
 * @param *stream   ビットストリームのハンドル
 * @param value     書き込む整数
 */
void bit_stream_write_signed_rice(bit_stream* stream, int32_t value, uint32_t parameter){
    uint32_t val;
    uint32_t quotient;
    uint32_t remainder;

    val = CONVERT_INT32_TO_UINT32(value);
    quotient = RSHIFT(val, parameter);
    remainder = val - LSHIFT(quotient, parameter);

    /* 商をunary符号として書き込む */
    bit_stream_write_unary(stream, quotient);

    /* 剰余を書き込む */
    bit_stream_write_uint(stream, remainder, parameter);
}

/*!
 * @brief			バッファに残っている、ファイルポインタが示すファイルに書き込まれていない値を無条件に書き込みます。
 * @param *stream	ビットストリームのハンドル
 */
void bit_stream_close(bit_stream* stream) {
    if (stream->mode == BIT_STREAM_MODE_WRITE) {
        write_buffer(stream);
    }

    stream->buffer_position = 0;
    stream->buffer = 0;
}