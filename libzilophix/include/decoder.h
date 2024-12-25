#ifndef DECODER_HEADER_INCLUDED
#define DECODER_HEADER_INCLUDED

#include "bit_stream.h"
#include "lms.h"
#include "block.h"
#include "code.h"
#include "tag.h"
#include "polynomial_predictor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*!
 * @brief デコーダ
 */
typedef struct {
    FILE* file;                                     /* ファイルハンドル */
    bit_stream* bit_stream;                         /* ビットストリームのハンドル */

    uint8_t format_version;                         /* フォーマットのバージョン */
    uint32_t sample_rate;                           /* サンプリング周波数 */
    uint8_t bits_per_sample;                        /* PCMの量子化ビット数 */
    uint8_t num_channels;                           /* チャンネル数 */
    uint32_t num_total_samples;                     /* ファイルに含まれるサンプルの総数 */
    uint8_t filter_taps;                            /* SSLMSフィルタのタップ数 */
    uint16_t block_size;                            /* ブロック（厳密にはサブブロック）に含まれるサンプル数*/
    bool use_mid_side_stereo;                       /* ミッドサイドステレオが使用されているかどうかを示すフラグ */
    uint32_t num_blocks;                            /* ファイルに含まれるブロックの総数 */
    uint8_t reserved1;                              /* 予約済み1 */
    uint8_t reserved2;                              /* 予約済み2 */
    uint32_t audio_data_begin;
    uint32_t audio_data_size;

    lms** lms_filters;                              /* チャンネル毎のSSLMSフィルタのハンドルを格納する領域 */
    polynomial_predictor** polynomial_predictors;   /* チャンネル毎の多項式予測器のハンドルを格納する領域 */

    tag* tag;                                       /* タグ情報のハンドル */
    code* coder;                                    /* ブロック読み書きAPIのハンドル */
    block* current_block;                           /* デコード中のブロックのハンドル */
    uint8_t current_read_sub_block_channel;         /* 次にサンプルを読み込む場合のチャンネルのオフセット */
    uint16_t current_read_sub_block_offset;         /* 次にサンプルを読み込む場合に参照するサブブロックのオフセット */

    uint32_t num_samples_read;                      /* 読み込み済みサンプル数 */
    bool is_seeking;                                /* シーク処理中であるかどうかを示すフラグ */
} decoder;

/*!
 * @brief                   デコーダのハンドルを生成します。
 * @param path              デコードするファイルのハンドル
 * @return                  デコーダのハンドル
 */
decoder* decoder_create(FILE* file);

/*!
 * @brief                   デコーダを解放します。
 * @param decoder           デコーダのハンドル
 */
void decoder_free(decoder* decoder);

/*!
 * @brief                   ファイルを閉じます。
 * @param decoder           デコーダのハンドル
 */
void decoder_close(decoder* decoder);

/*!
 * @brief                   次の1サンプルを読み込み、PCMサンプルとして返します。
 * @param decoder           デコーダのハンドル
 * @return                  デコードされたPCMサンプル
 */
int32_t decoder_read_sample(decoder* decoder);

/*!
 * @brief                   指定されたオフセットのサンプルまでシークします。
 * @param *decoder          デコーダのハンドル
 * @param sample_offset     シーク先のサンプルのオフセット
 */
void decoder_seek_sample_to(decoder* decoder, uint32_t sample_offset);

/*!
 * @brief                   ミリ秒単位で指定された時間までシークします。
 * @param *decoder          デコーダのハンドル
 * @param sample_offset     シーク先時間（ミリ秒）
 */
void decoder_seek_milliseconds_to(decoder* decoder, uint32_t ms);

/*!
 * @brief                   デコーダが開いているファイルの演奏時間をミリ秒単位で取得します。
 * @param *decoder          デコーダのハンドル
 * @return                  演奏時間（ミリ秒単位）
 */
uint32_t decoder_get_duration_ms(decoder* decoder);

#endif