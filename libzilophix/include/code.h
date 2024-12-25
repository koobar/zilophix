#ifndef CODE_HEADER_INCLUDED
#define CODE_HEADER_INCLUDED

#include "bit_stream.h"
#include "block.h"

typedef struct {
    bit_stream* bitstream;              /* 入出力用ビットストリームのハンドル */
    uint8_t bits_per_sample;
    uint8_t bits_of_entropy_parameter;
    uint8_t rice_parameter_max;
    uint8_t blank_partition_parameter;
    uint32_t* workA;                    /* 作業領域A */
    uint32_t* workB;                    /* 作業領域B */
} code;

/*!
 * @brief           ブロックを読み書きするAPIのハンドルを生成します。
 * @param *stream   ビットストリームのハンドル
 * @return          ブロック読み書きAPIのハンドル
 */
code* code_create(bit_stream* stream, uint8_t fmt_version, uint8_t bits_per_sample);

/*!
 * @brief           ブロックを読み書きするAPIのハンドルを解放します。
 * @param *coder    ブロック読み書きAPIのハンドル
 */
void code_free(code* coder);

/*!
 * @brief           ブロックを書き込みます。
 * @param *coder    ブロック読み書きAPIのハンドル
 * @param *block    書き込むブロックのハンドル
 */
void code_write_block(code* coder, const block* block);

/*!
 * @brief           ブロックを読み込みます。
 * @param *coder    ブロック読み書きAPIのハンドル
 * @param *block    読み込んだデータを格納するブロックのハンドル
 */
void code_read_block(code* coder, block* block);

#endif