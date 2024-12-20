#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/zilophix.h"
#include "./include/encoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include <stdlib.h>

/*!
 * @brief           マジックナンバーを書き込みます。
 * @param *encoder  エンコーダのハンドル
*/
static void write_magic_number(encoder* encoder){
    /* マジックナンバーを書き込む。 */
    write_uint8(encoder->output_file, 0x5A);
    write_uint8(encoder->output_file, 0x70);
    write_uint8(encoder->output_file, 0x58);
}

/*!
 * @brief           フォーマット情報を書き込みます。
 * @param *encoder  エンコーダのハンドル
*/
static void write_format_info(encoder* encoder){
    /* フォーマットのバージョンを書き込む。 */
    write_uint8(encoder->output_file, FORMAT_VERSION_CURRENT);

    /* PCMフォーマット情報を書き込む。 */
    write_uint32(encoder->output_file, encoder->sample_rate);
    write_uint8(encoder->output_file, encoder->bits_per_sample);
    write_uint8(encoder->output_file, encoder->num_channels);
    write_uint32(encoder->output_file, encoder->num_samples);

    /* エンコード情報 */
    write_uint8(encoder->output_file, encoder->filter_taps);
    write_uint16(encoder->output_file, encoder->block_size);
    write_bool(encoder->output_file, encoder->use_mid_side_stereo);
    write_uint32(encoder->output_file, encoder->num_blocks);

    /* 予約済みコードを書き込む（未実装は常にゼロ）*/
    write_uint8(encoder->output_file, 0x00);
    write_uint8(encoder->output_file, 0x00);

    /* オーディオデータの開始アドレスを書き込む領域を保持し、ダミーアドレスを書き込む
     * オーディオデータの開始位置を明確に記録することで、例えバージョンアップなどで
     * これ以降のデータ構造が変化しても、ここまでが正しければ、どこからが圧縮された
     * オーディオデータであるのか判別することができる。*/
    write_char(encoder->output_file, 'A');
    write_char(encoder->output_file, 'D');
    write_char(encoder->output_file, 'D');
    write_char(encoder->output_file, 'R');
    encoder->audio_data_position_position = (uint32_t)ftell(encoder->output_file);
    write_uint32(encoder->output_file, 0xFFFFFFFF);

    /* オーディオデータのサイズを書き込む領域を保持し、ダミーサイズを書き込む
     * オーディオデータのサイズを明確に記録することで、例えバージョンアップなどで
     * これ以降のデータ構造が変化しても、ここまでが正しければ、圧縮された
     * オーディオデータのサイズを判別することができる。*/
    write_char(encoder->output_file, 'S');
    write_char(encoder->output_file, 'I');
    write_char(encoder->output_file, 'Z');
    write_char(encoder->output_file, 'E');
    encoder->audio_data_size_position = (uint32_t)ftell(encoder->output_file);
    write_uint32(encoder->output_file, 0xFFFFFFFF);
}

/*!
 * @brief           ヘッダ部の書き込みを行います。
 * @param *encoder  エンコーダのハンドル
 */
static void write_header(encoder* encoder) {
    /* マジックナンバーを書き込む。 */
    write_magic_number(encoder);

    /* フォーマット情報を書き込む */
    write_format_info(encoder);
    
    /* タグ情報を書き込む */
    tag_write(encoder->output_file, encoder->tag);

    /* データの開始オフセットを保持 */
    encoder->audio_data_begin_position = (uint32_t)ftell(encoder->output_file);
}

/*!
 * @brief           オーディオデータ部分の書き込みを終了し、データの開始アドレスとサイズを書き込みます。
 * @param *encoder  エンコーダのハンドル
*/
static void flush_audio_data(encoder* encoder){
    uint32_t current_len = (uint32_t)ftell(encoder->output_file);
    uint32_t data_size = current_len - (encoder->audio_data_begin_position);

    /* オーディオデータの開始アドレスのアドレスに移動し、開始アドレスを書き込む */
    fseek(encoder->output_file, encoder->audio_data_position_position, SEEK_SET);
    write_uint32(encoder->output_file, encoder->audio_data_begin_position);

    /* オーディオデータのサイズを書き込む位置に移動し、サイズを書き込む */
    fseek(encoder->output_file, encoder->audio_data_size_position, SEEK_SET);
    write_uint32(encoder->output_file, data_size);

    /* 元の位置に移動 */
    fseek(encoder->output_file, current_len, SEEK_SET);
}

/*!
 * @brief           ブロックをエンコードします。ミッドサイドステレオが使用されている場合、encode_current_midside_block関数を使用してください。
 * @param *encoder  エンコーダのハンドル
 */
static void encode_current_block_normal(encoder* encoder){
    uint8_t ch;
    uint16_t offset;
    int32_t sample, residual;
    sub_block* sb = NULL;
    polynomial_predictor* poly = NULL;
    lms* lms = NULL;

    for (ch = 0; ch < encoder->num_channels; ++ch) {
        sb = encoder->current_block->sub_blocks[ch];
        poly = encoder->polynomial_predictors[ch];
        lms = encoder->lms_filters[ch];

        for (offset = 0; offset < sb->size; ++offset) {
            /* STEP 1. 多項式予測 */
            residual = sb->samples[offset] - polynomial_predictor_predict(poly);
            polynomial_predictor_update(poly, sb->samples[offset]);

            /* STEP 2. SSLMSフィルタによる予測 */
            sample = residual;
            residual -= lms_predict(lms);
            lms_update(lms, sample, residual);

            /* STEP 3. 出力 */
            sb->samples[offset] = residual;
        }
    }
}

/*!
 * @brief           ミッドサイドステレオブロックをエンコードします。ミッドサイドステレオではない場合、encode_current_normal_block関数を使用してください。
 * @param *encoder  エンコーダのハンドル
 */
static void encode_current_block_midside(encoder* encoder){
    uint16_t offset;
    int32_t left;
    int32_t right;
    int32_t mid;
    int32_t side;
    sub_block* lch = encoder->current_block->sub_blocks[0];
    sub_block* rch = encoder->current_block->sub_blocks[1];
    polynomial_predictor* lpoly = encoder->polynomial_predictors[0];
    polynomial_predictor* rpoly = encoder->polynomial_predictors[1];
    lms* llms = encoder->lms_filters[0];
    lms* rlms = encoder->lms_filters[1];

    for (offset = 0; offset < encoder->block_size; ++offset){
        left = lch->samples[offset];
        right = rch->samples[offset];

        /* STEP 1. ミッドサイドステレオへの変換 */
        lch->samples[offset] = mid = RSHIFT(left + right, 1);
        rch->samples[offset] = side = left - right;

        /* STEP 2. 多項式予測 */
        mid -= polynomial_predictor_predict(lpoly);
        side -= polynomial_predictor_predict(rpoly);
        polynomial_predictor_update(lpoly, lch->samples[offset]);
        polynomial_predictor_update(rpoly, rch->samples[offset]);
        lch->samples[offset] = mid;
        rch->samples[offset] = side;

        /* STEP 3. SSLMSフィルタによる予測 */
        mid -= lms_predict(llms);
        side -= lms_predict(rlms);
        lms_update(llms, lch->samples[offset], mid);
        lms_update(rlms, rch->samples[offset], side);

        /* STEP 4. 出力 */
        lch->samples[offset] = mid;
        rch->samples[offset] = side;
    }
}

/*!
 * @brief           指定されたハンドルのエンコーダで読み込まれたブロックのエンコードを行います。
 * @param *encoder  エンコーダのハンドル
 */
static void encode_current_block(encoder* encoder) {
    if (encoder->use_mid_side_stereo){
        encode_current_block_midside(encoder);
    }
    else{
        encode_current_block_normal(encoder);
    }
}

/*!
 * @brief               ブロック数を計算します
 * @param num_samples   サンプル数
 * @param num_channels  チャンネル数
 * @param block_size    サブブロックのサンプル数
 * @return              ブロック数
 */
static uint32_t compute_block_count(uint32_t num_samples, uint8_t num_channels, uint16_t block_size) {
    uint32_t num_samples_per_ch = num_samples / num_channels;
    uint32_t num_blocks = num_samples_per_ch / block_size;

    if (num_samples_per_ch % block_size != 0) {
        ++num_blocks;
    }

    return num_blocks;
}

/*!
 * @brief                           エンコーダを初期化します
 * @param *encoder                  エンコーダのハンドル
 * @param *file                     出力先のファイルハンドル
 * @param sample_rate               サンプリング周波数
 * @param bits_per_sample           量子化ビット数
 * @param num_channels              チャンネル数
 * @param num_samples               合計サンプル数
 * @param block_size                ブロックサイズ
 * @param use_mid_side_stereo       ミッドサイドステレオを使用するかどうかを示すフラグ
 * @param filter_taps               LMSフィルタのタップ数
 * @param *tag                      タグ情報
 */
static void init(
    encoder* encoder, 
    FILE* file, 
    uint32_t sample_rate, 
    uint8_t bits_per_sample, 
    uint8_t num_channels, 
    uint32_t num_samples, 
    uint16_t block_size, 
    bool use_mid_side_stereo, 
    uint8_t filter_taps,
    tag* tag) {
    uint8_t ch;

    if (filter_taps > LMS_MAX_TAPS) {
        filter_taps = LMS_MAX_TAPS;
    }

    encoder->output_file = file;
    encoder->output_bit_stream = bit_stream_create(encoder->output_file, BIT_STREAM_MODE_WRITE);
    encoder->sample_rate = sample_rate;
    encoder->bits_per_sample = bits_per_sample;
    encoder->num_channels = num_channels;
    encoder->num_samples = num_samples;
    encoder->filter_taps = filter_taps;
    encoder->block_size = block_size;
    encoder->use_mid_side_stereo = use_mid_side_stereo;
    encoder->num_blocks = compute_block_count(num_samples, num_channels, block_size);
    encoder->lms_filters = (lms**)malloc(sizeof(lms*) * num_channels);
    encoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * num_channels);
    encoder->coder = code_create(encoder->output_bit_stream, FORMAT_VERSION_CURRENT, encoder->bits_per_sample);
    encoder->current_block = (block*)malloc(sizeof(block));
    encoder->current_sub_block_channel = 0;
    encoder->current_sub_block_offset = 0;
    encoder->tag = tag;

    /* ブロックを初期化 */
    block_init(encoder->current_block, block_size, num_channels);

    /* 各チャンネル用のフィルタを初期化 */
    for (ch = 0; ch < num_channels; ++ch) {
        encoder->lms_filters[ch] = lms_create(filter_taps, bits_per_sample);
        encoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }

    /* ヘッダ部を書き込む */
    write_header(encoder);
}

/*!
 * @brief                           指定された設定で、エンコーダのハンドルを生成します。
 * @param *file                     出力先のファイルハンドル
 * @param sample_rate               サンプリング周波数
 * @param bits_per_sample           量子化ビット数
 * @param num_channels              チャンネル数
 * @param num_samples               合計サンプル数
 * @param block_size                ブロックサイズ
 * @param use_mid_side_stereo       ミッドサイドステレオを使用するかどうかを示すフラグ
 * @param filter_taps               LMSフィルタのタップ数
 * @param *tag                      タグ情報
 */
encoder* encoder_create(
    FILE* file,
    uint32_t sample_rate,
    uint8_t bits_per_sample,
    uint8_t num_channels,
    uint32_t num_samples,
    uint16_t block_size,
    bool use_mid_side_stereo,
    uint8_t filter_taps,
    tag* tag) {
    encoder* result = (encoder*)malloc(sizeof(encoder));

    if (result == NULL) {
        report_error(ERROR_ENCODER_CANNOT_ALLOCATE_MEMORY);
        return NULL;
    }

    init(
        result,
        file,
        sample_rate,
        bits_per_sample,
        num_channels,
        num_samples,
        block_size,
        use_mid_side_stereo,
        filter_taps,
        tag);

    return result;
}

/*!
 * @brief           指定されたハンドルのエンコーダを解放します。
 * @param *encoder  エンコーダのハンドル
 */
void encoder_free(encoder* encoder) {
    uint8_t ch;

    /* 各チャンネル用のフィルタを解放 */
    for (ch = 0; ch < encoder->num_channels; ++ch) {
        lms_free(encoder->lms_filters[ch]);
        polynomial_predictor_free(encoder->polynomial_predictors[ch]);
    }

    free(encoder->output_bit_stream);
    free(encoder->lms_filters);
    free(encoder->polynomial_predictors);
    free(encoder->coder);

    block_free(encoder->current_block);
    code_free(encoder->coder);
    tag_free(encoder->tag);
}

/*!
 * @brief           指定されたハンドルのエンコーダで、指定されたサンプルをエンコードします。
 * @param *encoder  エンコーダのハンドル
 * @param sample    サンプル
 */
void encoder_write_sample(encoder* encoder, int32_t sample) {
    int32_t flg_encode_block = 0;

    encoder->current_block->sub_blocks[encoder->current_sub_block_channel++]->samples[encoder->current_sub_block_offset] = sample;

    /* オフセット更新 */
    if (encoder->current_sub_block_channel >= encoder->num_channels) {
        encoder->current_sub_block_channel = 0;
        ++encoder->current_sub_block_offset;

        /* サブブロックのオフセットがブロックサイズ以上となった際は、サブブロックのオフセットをゼロに戻し、エンコードされたブロックを出力ストリームに書き込む */
        if (encoder->current_sub_block_offset >= encoder->block_size) {
            encoder->current_sub_block_offset = 0;
            flg_encode_block = 1;
        }
    }

    /* 出力ストリームへの書き込みフラグが立っていれば書き込む */
    if (flg_encode_block == 1) {
        encode_current_block(encoder);
        code_write_block(encoder->coder, encoder->current_block);
    }
}

/*!
 * @brief           指定されたハンドルのエンコーダでのエンコードの終了処理を行います。すべてのサンプルのエンコードが終了した後に、必ず呼び出してください。
 * @param *encoder  エンコーダのハンドル
 */
void encoder_end_write(encoder* encoder) {
    if (encoder->current_sub_block_offset != 0) {
        encode_current_block(encoder);
        code_write_block(encoder->coder, encoder->current_block);
    }

    bit_stream_close(encoder->output_bit_stream);

    flush_audio_data(encoder);
    fflush(encoder->output_file);
    fclose(encoder->output_file);
}