#include "./include/macro.h"
#include "./include/code.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include "./include/zilophix.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define RESTORE_PARTITION_COUNT(partition_parameter)            (LSHIFT(1, partition_parameter))
#define RESTORE_PARTITION_SIZE(data_size, partition_parameter)  ((data_size) / RESTORE_PARTITION_COUNT(partition_parameter))

#define PARTITION_PARAMETER_MIN                 1
#define PARTITION_PARAMETER_MAX                 PARTITION_PARAMETER_MIN + 3
#define PARTITION_PARAMETER_NEED_BITS           2
#define PARTITION_COUNT_MAX                     RESTORE_PARTITION_COUNT(PARTITION_PARAMETER_MAX)

#define RICE_PARAMETER_MAX_FOR_PCM16            14
#define BLANK_PARAMETER_FOR_PCM16               15
#define ENTROPY_PARAMETER_NEED_BITS_FOR_PCM16   4

#define RICE_PARAMETER_MAX_FOR_PCM24            30
#define BLANK_PARAMETER_FOR_PCM24               31
#define ENTROPY_PARAMETER_NEED_BITS_FOR_PCM24   5

#ifdef _MSC_VER

/* MSVC には __builtin_clz が含まれないため、自前で実装する */
#define __builtin_clz(x) clz(x)

static inline uint32_t popcnt(uint32_t x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;
}

static inline uint32_t clz(uint32_t x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return 32 - popcnt(x);
}

#endif

/*!
 * @brief                   指定されたデータを指定されたパラメータでライス符号化した場合のビット数を計算します。
 * @param *data             データ
 * @param data_size         データのサイズ
 * @param parameter         ライス符号化のパラメータ
 * @return                  指定されたデータを、指定されたパラメータでライス符号化した場合のビット数
 */
static uint32_t compute_rice_total_bits(const int32_t* data, const uint16_t start, const uint16_t data_size, const uint32_t parameter) {
    uint32_t estimated_size = 0;
    uint32_t val;
    uint32_t quotient;
    uint16_t offset;

    for (offset = 0; offset < data_size; ++offset) {
        val = CONVERT_INT32_TO_UINT32(data[start + offset]);
        quotient = RSHIFT(val, parameter);

        estimated_size += quotient;
        estimated_size += 1;
    }

    estimated_size += (parameter * data_size);

    return estimated_size;
}

/*!
 * @brief               指定されたデータに対して最適となるライス符号化のパラメータを計算します。
 * @param *coder        符号化器のハンドル
 * @param *data         データのポインタ
 * @param start         パーティションの開始オフセット
 * @param data_size     パーティションのサイズ
 * @param *total_bits   [出力]求められたパラメータでライス符号化した場合のデータの合計ビット数
 * @return              指定されたデータに対して最適となるライス符号化のパラメータ
 */
static uint32_t compute_optimal_rice_parameter(code* coder, const int32_t* data, uint16_t start, uint16_t data_size, uint32_t* total_bits) {
    double sum = 0;
    double mean = 0;
    uint32_t i, imean, parameter;

    /* 出現するデータの絶対値の平均を求める。*/
    for (i = 0; i < data_size; ++i) {
        sum += abs(data[start + i]);
    }
    mean = sum / data_size;

    /* 平均を整数に丸め込んだ値の表現に必要なビット数をパラメータとする。*/
    imean = CLAMP((uint32_t)floor(mean), 0, UINT32_MAX);
    if (imean == 0){
        /* 平均がゼロの場合、剰余をゼロビット出力(=無出力)でもOK */
        parameter = 0;
    }
    else{
        /* 平均がゼロ以外の場合、最適パラメータは (32-平均の表現に必要な最小ビット数) で推定可能 */
        parameter = CLAMP(32 - __builtin_clz(imean), 0, coder->rice_parameter_max);
    }
    
    *(total_bits) = compute_rice_total_bits(data, start, data_size, parameter);
    return parameter;
}

/*!
 * @brief                   パーティションで使用すべきエントロピー符号化のパラメータを計算します。
 * @param *coder            符号化器のハンドル
 * @param *data             データ全体のポインタ
 * @param start             パーティションの開始位置
 * @param partition_size    パーティションのサイズ
 * @param *partition_bits   [出力]求められたエントロピー符号化のパラメータを使用してエントロピー符号化した場合のパーティション全体のビット数
 * @return                  最適なエントロピー符号化パラメータ
 */
static uint32_t compute_optimal_entropy_parameter(code* coder, const int32_t* data, uint16_t start, uint16_t partition_size, uint32_t* partition_bits) {
    bool is_blank_partition = true;
    uint16_t offset;

    /* ブランクパーティション（すべての値がゼロであるパーティション）であるか判定する。*/
    for (offset = 0; offset < partition_size; ++offset) {
        if (data[offset + start] != 0) {
            is_blank_partition = false;
            break;
        }
    }

    if (is_blank_partition) {
        *(partition_bits) = coder->bits_of_entropy_parameter;
        return coder->blank_partition_parameter;
    }

    return compute_optimal_rice_parameter(coder, data, start, partition_size, partition_bits);
}

/*!
 * @brief               指定されたデータに対して最適となるパーティションパラメータ、および、それぞれのパーティションで使用すべきエントロピー符号化のパラメータを計算します。
 * @param *coder        符号化器のハンドル
 * @param *data         データのポインタ
 * @param data_size     データのサイズ
 * @return              指定されたデータに対して最適となるパーティションパラメータ
 */
static uint32_t compute_optimal_partition_parameter(code* coder, const int32_t* data, uint16_t data_size) {
    uint32_t partition_size;
    uint32_t partition_index;
    uint32_t trial_pp;
    uint32_t partition_count;
    uint32_t optimal_partition_parameter;
    uint32_t size, min_size;
    uint16_t start;
    uint32_t partition_bits;

    min_size = UINT32_MAX;
    optimal_partition_parameter = 0;

    for (trial_pp = PARTITION_PARAMETER_MIN; trial_pp <= PARTITION_PARAMETER_MAX; ++trial_pp) {
        partition_count = RESTORE_PARTITION_COUNT(trial_pp);
        partition_size = data_size / partition_count;
        size = 0;

        /* それぞれのパーティションにとって最適となるエントロピー符号化のパラメータを求め、作業領域に格納する */
        for (partition_index = 0; partition_index < partition_count; ++partition_index) {
            start = partition_size * partition_index;

            /* このパーティションのエントロピー符号化のパラメータを作業領域に保存 */
            coder->workB[partition_index] = compute_optimal_entropy_parameter(coder, data, start, partition_size, &partition_bits);
            
            /* パーティションのサイズを加算 */
            size += partition_bits;
        }

        /* パーティションのサイズに、エントロピー符号化のパラメータを保存するために要するビット数を加算 */
        size += PARTITION_PARAMETER_NEED_BITS;

        /* すでに発見されたパーティションの最小サイズより、今回試したパラメータによる
         * パーティションサイズのほうが小さければ、最小サイズを置き換え、作業領域に格納された、
         * すべてのパーティションのエントロピー符号化のパラメータを、出力領域にコピーする。*/
        if (min_size > size) {
            min_size = size;
            optimal_partition_parameter = trial_pp;
            memcpy(coder->workA, coder->workB, sizeof(uint32_t) * PARTITION_COUNT_MAX);
        }
    }

    return optimal_partition_parameter;
}

/*!
 * @brief                   Rice符号を用いてパーティションに含まれる予測残差をビットストリームに書き込みます。
 * @param *stream           ビットストリームのハンドル
 * @param *data             書き込む予測残差のポインタ
 * @param start             パーティションの開始オフセット
 * @param partition_size    パーティションのサイズ
 * @param parameter         ライス符号化のパラメータ
 */
static void write_rice_partition(bit_stream* stream, const int32_t* data, uint16_t start, uint16_t partition_size, uint32_t parameter) {
    uint16_t offset;

    for (offset = 0; offset < partition_size; ++offset) {
        bit_stream_write_signed_rice(stream, data[offset + start], parameter);
    }
}

/*!
 * @brief                   Rice符号が使用されたパーティションに含まれる予測残差を読み込みます。
 * @param *stream           ビットストリームのハンドル
 * @param *data             読み込まれた予測残差を格納する領域のポインタ
 * @param start             パーティションの開始オフセット
 * @param partition_size    パーティションのサイズ
 * @param parameter         ライス符号化のパラメータ
 */
static void read_rice_partition(bit_stream* stream, int32_t* data, uint16_t start, uint16_t partition_size, uint32_t parameter) {
    uint16_t offset;
    int32_t* pdata = data + start;

    for (offset = 0; offset < partition_size; ++offset) {
        *(pdata++) = bit_stream_read_rice_signed(stream, parameter);
    }
}

/*!
 * @brief               指定されたビットストリームに、指定されたサブブロックをライス符号化して書き込みます。
 * @param *coder        符号化器のハンドル
 * @param *sub_block    サブブロックのハンドル
 */
static void write_sub_block(code* coder, const sub_block* sub_block) {
    uint32_t p, start;
    uint32_t parameter;
    uint16_t partition_size;
    uint32_t partition_parameter;
    uint32_t partition_count;

    /* ライス符号化のパーティションパラメータを計算 */
    partition_parameter = compute_optimal_partition_parameter(coder, sub_block->samples, sub_block->size);

    /* パーティションパラメータからパーティション数とパーティションのサイズを計算 */
    partition_count = RESTORE_PARTITION_COUNT(partition_parameter);
    partition_size = sub_block->size / partition_count;

    /* パーティションパラメータを保存する */
    bit_stream_write_uint(coder->bitstream, partition_parameter - PARTITION_PARAMETER_MIN, PARTITION_PARAMETER_NEED_BITS);

    /* サブブロックをエントロピー符号化して書き込む */
    for (p = 0; p < partition_count; ++p) {
        start = partition_size * p;
        parameter = coder->workA[p];

        /* エントロピー符号化のパラメータを書き込む */
        bit_stream_write_uint(coder->bitstream, (uint32_t)(parameter), coder->bits_of_entropy_parameter);

        /* エントロピー符号のパラメータがRice符号の範囲内なら、Rice符号でパーティションを書き込む */
        if (parameter <= coder->rice_parameter_max) {
            write_rice_partition(coder->bitstream, sub_block->samples, start, partition_size, parameter);
        }
    }
}

/*!
 * @brief               指定されたビットストリームから、ライス符号化されて書き込まれたサブブロックのデータを読み込み、指定されたサブブロックに格納します。
 * @param *coder        符号化器のハンドル
 * @param *sub_block    サブブロックのハンドル
 */
static void read_sub_block(code* coder, sub_block* sub_block) {
    uint32_t parameter;
    uint16_t offset;
    uint32_t p, start;
    uint16_t partition_size;
    uint32_t partition_parameter;
    uint32_t partition_count;

    /* パーティションパラメータを取得 */
    partition_parameter = bit_stream_read_uint(coder->bitstream, PARTITION_PARAMETER_NEED_BITS) + PARTITION_PARAMETER_MIN;

    /* ライス符号化のパーティション数とパーティションサイズを計算 */
    partition_count = RESTORE_PARTITION_COUNT(partition_parameter);
    partition_size = sub_block->size / partition_count;

    /* エントロピー符号化された整数を読み込む */
    for (p = 0; p < partition_count; ++p) {
        start = p * partition_size;

        /* エントロピー符号化のパラメータを取得 */
        parameter = bit_stream_read_uint(coder->bitstream, coder->bits_of_entropy_parameter);

        if (parameter <= coder->rice_parameter_max){
            read_rice_partition(coder->bitstream, sub_block->samples, start, partition_size, parameter);
        }
        else {
            for (offset = 0; offset < partition_size; ++offset) {
                sub_block->samples[start + offset] = 0;
            }
        }
    }
}

/*!
 * @brief               ブロックを読み書きするAPIのハンドルを生成します。
 * @param *stream       ビットストリームのハンドル
 * @param fmt_version   フォーマットのバージョン
 * @return              ブロック読み書きAPIのハンドル
 */
code* code_create(bit_stream* stream, uint8_t fmt_version, uint8_t bits_per_sample) {
    code* result = (code*)malloc(sizeof(code));

    if (result == NULL) {
        return NULL;
    }

    result->bitstream = stream;
    result->bits_per_sample = bits_per_sample;
    result->workA = (uint32_t*)calloc(PARTITION_COUNT_MAX, sizeof(uint32_t));
    result->workB = (uint32_t*)calloc(PARTITION_COUNT_MAX, sizeof(uint32_t));

    if (fmt_version == FORMAT_VERSION_1_0){
        /* 16ビットPCMデータ処理時には4ビット範囲のエントロピー符号が、24ビットPCMデータ処理時には5ビット範囲のエントロピー符号化パラメータを用いる */
        if (bits_per_sample == 16){
            result->rice_parameter_max = RICE_PARAMETER_MAX_FOR_PCM16;
            result->blank_partition_parameter = BLANK_PARAMETER_FOR_PCM16;
            result->bits_of_entropy_parameter = ENTROPY_PARAMETER_NEED_BITS_FOR_PCM16;
        }
        else if (bits_per_sample == 24){
            result->rice_parameter_max = RICE_PARAMETER_MAX_FOR_PCM24;
            result->blank_partition_parameter = BLANK_PARAMETER_FOR_PCM24;
            result->bits_of_entropy_parameter = ENTROPY_PARAMETER_NEED_BITS_FOR_PCM24;
        }
    }
    else{
        report_error(ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION);
    }

    return result;
}

/*!
 * @brief           ブロックを読み書きするAPIのハンドルを解放します。
 * @param *coder    ブロック読み書きAPIのハンドル
 */
void code_free(code* coder) {
    free(coder->workA);
    free(coder->workB);
}

/*!
 * @brief           ブロックを書き込みます。
 * @param *coder    ブロック読み書きAPIのハンドル
 * @param *block    書き込むブロックのハンドル
 */
void code_write_block(code* coder, const block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        write_sub_block(coder, block->sub_blocks[ch]);
    }
}

/*!
 * @brief           ブロックを読み込みます。
 * @param *coder    ブロック読み書きAPIのハンドル
 * @param *block    読み込んだデータを格納するブロックのハンドル
 */
void code_read_block(code* coder, block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        read_sub_block(coder, block->sub_blocks[ch]);
    }
}