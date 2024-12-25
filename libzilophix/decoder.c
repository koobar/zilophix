#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/decoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"

const static uint8_t supported_format_versions[2] = { 0x01, 0x10 };

/*! 
 * @brief           指定されたバージョン番号が、このデコーダでサポートされているフォーマットバージョンであるかを判定します。
 * @param version   バージョン番号
 * @return          サポートされているバージョンなら true を、そうでなければ false を返します
 */
static bool is_supported_version(uint8_t version) {
    size_t n = sizeof(supported_format_versions) / sizeof(supported_format_versions[0]);
    size_t i;

    for (i = 0; i < n; ++i) {
        if (supported_format_versions[i] == version) {
            return true;
        }
    }

    return false;
}

/*!
 * @brief           開かれているファイルがZilophiXフォーマットであるかどうかを、マジックナンバーを以て確認します。
 * @param *decoder  デコーダのハンドル
 * @return          開かれているファイルがZilophiXフォーマットであればtrueを、それ以外のフォーマットであればfalseを返します。
 */
static bool check_magic_number(decoder* decoder){
    return  read_uint8(decoder->file) == 0x5A &&
            read_uint8(decoder->file) == 0x70 &&
            read_uint8(decoder->file) == 0x58;
}

/*!
 * @brief           フォーマット情報を読み込みます。
 * @param *decoder  デコーダのハンドル
 */
static void read_format_info(decoder* decoder){
    /* フォーマットバージョンを読み込む */
    decoder->format_version = read_uint8(decoder->file);

    if (!is_supported_version(decoder->format_version)) {
        report_error(ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION);
        return;
    }

    /* PCMフォーマット情報を読み込む */
    decoder->sample_rate = read_uint32(decoder->file);
    decoder->bits_per_sample = read_uint8(decoder->file);
    decoder->num_channels = read_uint8(decoder->file);
    decoder->num_total_samples = read_uint32(decoder->file);

    /* Zilophixエンコード情報を読み込む */
    decoder->filter_taps = read_uint8(decoder->file);
    decoder->block_size = read_uint16(decoder->file);
    decoder->use_mid_side_stereo = read_bool(decoder->file);
    decoder->num_blocks = read_uint32(decoder->file);

    /* 予約済みコードを読み込む（未使用ないし未実装ならゼロ） */
    decoder->reserved1 = read_uint8(decoder->file);
    decoder->reserved2 = read_uint8(decoder->file);

    /* オーディオデータの開始オフセットを読み込む */
    if (read_char(decoder->file) == 'A' && read_char(decoder->file) == 'D' && read_char(decoder->file) == 'D' && read_char(decoder->file) == 'R'){
        decoder->audio_data_begin = read_uint32(decoder->file);
    }

    /* オーディオデータのサイズを読み込む */
    if (read_char(decoder->file) == 'S' && read_char(decoder->file) == 'I' && read_char(decoder->file) == 'Z' && read_char(decoder->file) == 'E'){
        decoder->audio_data_size = read_uint32(decoder->file);
    }
}

/*!
 * @brief           指定されたハンドルのデコーダで開かれたファイルから、Zilophixファイルのヘッダ部を読み込みます。
 * @param *decoder  デコーダのハンドル
 */
static void read_header(decoder* decoder) {
    if (check_magic_number(decoder)){
        /* フォーマット情報を読み込む */
        read_format_info(decoder);

        /* タグ情報を読み込む */
        tag_read(decoder->file, &decoder->tag);
    }
    else {
        /* マジックナンバーが不正な値である旨のエラーをレポートする */
        report_error(ERROR_DECODER_INVALID_MAGIC_NUMBER);
    }
}

/*!
 * @brief           モノラルのブロックや、ミッドサイドステレオが使用されていないブロックをデコードします。
 * @param *decoder  デコーダのハンドル
 */
static void decode_normal_block(decoder* decoder){
    uint8_t ch;
    sub_block* sb = NULL;
    lms* lms = NULL;
    polynomial_predictor* poly = NULL;
    uint16_t offset;
    int32_t residual;
    int32_t sample;

    for (ch = 0; ch < decoder->num_channels; ++ch) {
        sb = decoder->current_block->sub_blocks[ch];
        lms = decoder->lms_filters[ch];
        poly = decoder->polynomial_predictors[ch];

        for (offset = 0; offset < sb->size; ++offset) {
            /* STEP 1. SSLMSフィルタを適用し、多項式予測器の予測残差を復元 */
            residual = sb->samples[offset];
            sample = residual + lms_predict(lms);
            lms_update(lms, sample, residual);

            /* STEP 2. 多項式予測器で予測された信号に予測残差を加算し、元の信号を復元 */
            sample += polynomial_predictor_predict(poly);
            polynomial_predictor_update(poly, sample);

            /* STEP 3. 復元された信号をサブブロックに格納 */
            sb->samples[offset] = sample;
        }
    }
}

/*!
 * @brief           ステレオで、ミッドサイドステレオが使用されているブロックをデコードします。
 * @param *decoder  デコーダのハンドル
 */
static void decode_midside_block(decoder* decoder){
    sub_block* lch = decoder->current_block->sub_blocks[0];
    sub_block* rch = decoder->current_block->sub_blocks[1];
    lms* llms = decoder->lms_filters[0];
    lms* rlms = decoder->lms_filters[1];
    polynomial_predictor* lpoly = decoder->polynomial_predictors[0];
    polynomial_predictor* rpoly = decoder->polynomial_predictors[1];
    int32_t left;
    int32_t right;
    int32_t mid;
    int32_t side;
    uint16_t offset;

    for (offset = 0; offset < decoder->block_size; ++offset) {
        /* STEP 1.  SSLMSフィルタを適用し、左右チャンネルの多項式予測器の予測残差を復元 */
        mid = lch->samples[offset] + lms_predict(llms);
        side = rch->samples[offset] + lms_predict(rlms);
        lms_update(llms, mid, lch->samples[offset]);
        lms_update(rlms, side, rch->samples[offset]);

        /* STEP 2. 多項式予測器で予測された信号に予測残差を加算し、元の信号を復元 */
        mid += polynomial_predictor_predict(lpoly);
        side += polynomial_predictor_predict(rpoly);
        polynomial_predictor_update(lpoly, mid);
        polynomial_predictor_update(rpoly, side);

        /* STEP 3. ミッドサイドステレオをLRステレオに変換する */
        mid = LSHIFT(mid, 1) | (side & 1);
        left = RSHIFT(mid + side, 1);
        right = RSHIFT(mid - side, 1);

        /* STEP 4. デコードされたサンプルをサブブロックに格納 */
        lch->samples[offset] = left;
        rch->samples[offset] = right;
    }
}

/*!
 * @brief           指定されたデコーダで読み込み済みのブロックのデコードを行います。
 * @param *decoder  デコーダのハンドル
 */
static void decode_current_block(decoder* decoder) {
    if (decoder->num_channels == 2 && decoder->use_mid_side_stereo){
        decode_midside_block(decoder);
    }
    else{
        decode_normal_block(decoder);
    }
}

/*!
 * @brief           指定されたデコーダを、指定されたパスのファイルをデコードできるように初期化します。
 * @param *decoder  デコーダのハンドル
 * @param *path     ファイルのパス
 */
static void decoder_init(decoder* decoder, FILE* file) {
    uint8_t ch;

    /* ビットストリームの初期化 */
    decoder->file = file;
    decoder->bit_stream = bit_stream_create(decoder->file, BIT_STREAM_MODE_READ);

    /* ヘッダ部を読み込む */
    read_header(decoder);

    decoder->lms_filters = (lms**)malloc(sizeof(lms*) * decoder->num_channels);
    decoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * decoder->num_channels);
    decoder->coder = code_create(decoder->bit_stream, decoder->format_version, decoder->bits_per_sample);
    decoder->current_block = (block*)malloc(sizeof(block));
    decoder->current_read_sub_block_channel = 0;
    decoder->current_read_sub_block_offset = 0;
    decoder->num_samples_read = 0;
    decoder->is_seeking = false;

    /* ブロックを初期化 */
    block_init(decoder->current_block, decoder->block_size, decoder->num_channels);

    /* 領域の確保に成功していれば、初期化を行う */
    for (ch = 0; ch < decoder->num_channels; ++ch) {
        decoder->lms_filters[ch] = lms_create(decoder->filter_taps, decoder->bits_per_sample);
        decoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }
}

/*!
 * @brief       デコーダのハンドルを生成します。
 * @param path  デコードするファイルのハンドル
 * @return      デコーダのハンドル
 */
decoder* decoder_create(FILE* file) {
    decoder* result = (decoder*)malloc(sizeof(decoder));

    /* 初期化処理 */
    decoder_init(result, file);
    
    /* オーディオデータの開始位置にシーク */
    fseek(result->file, result->audio_data_begin, SEEK_SET);
    
    return result;
}

/*!
 * @brief           デコーダを解放します。
 * @param decoder   デコーダのハンドル
 */
void decoder_free(decoder* decoder) {
    uint8_t ch;

    /* 各チャンネル用のフィルタを解放 */
    for (ch = 0; ch < decoder->num_channels; ++ch) {
        lms_free(decoder->lms_filters[ch]);
        polynomial_predictor_free(decoder->polynomial_predictors[ch]);
    }

    free(decoder->bit_stream);
    free(decoder->lms_filters);
    free(decoder->polynomial_predictors);
    free(decoder->coder);
    block_free(decoder->current_block);
    code_free(decoder->coder);
}

/*!
 * @brief           ファイルを閉じます。
 * @param decoder   デコーダのハンドル
 */
void decoder_close(decoder* decoder) {
    tag_free(decoder->tag);
    fclose(decoder->file);
}

/*!
 * @brief           強制的に次の1サンプルを読み込み、PCMサンプルとして返します。
 * @param decoder   デコーダのハンドル
 * @return          デコードされたPCMサンプル
 */
static int32_t force_read_sample(decoder* decoder) {
    int32_t sample;

    if (decoder->current_read_sub_block_offset == 0 && decoder->current_read_sub_block_channel == 0) {
        code_read_block(decoder->coder, decoder->current_block);
        decode_current_block(decoder);
    }

    sample = (int32_t)decoder->current_block->sub_blocks[decoder->current_read_sub_block_channel++]->samples[decoder->current_read_sub_block_offset];

    if (decoder->current_read_sub_block_channel == decoder->num_channels) {
        decoder->current_read_sub_block_channel = 0;
        ++decoder->current_read_sub_block_offset;

        if (decoder->current_read_sub_block_offset == decoder->block_size) {
            decoder->current_read_sub_block_offset = 0;
        }
    }
    decoder->num_samples_read += 1;

    return sample;
}

/*!
 * @brief           次の1サンプルを読み込み、PCMサンプルとして返します。
 * @param decoder   デコーダのハンドル
 * @return          デコードされたPCMサンプル
 */
int32_t decoder_read_sample(decoder* decoder) {
    if (decoder->is_seeking) {
        return 0;
    }

    if (decoder->num_samples_read >= decoder->num_total_samples) {
        return 0;
    }

    return force_read_sample(decoder);
}

/*!
 * @brief                   指定されたオフセットのサンプルまでシークします。
 * @param *decoder          デコーダのハンドル
 * @param sample_offset     シーク先のサンプルのオフセット
 */
void decoder_seek_sample_to(decoder* decoder, uint32_t sample_offset) {
    uint8_t ch;
    uint32_t offset;

    /* シーク中フラグを立てる */
    decoder->is_seeking = true;

    /* 後方シーク（再生位置を過去に戻す）の場合、ファイルの最初からデコードをやり直す。*/
    if (sample_offset < decoder->num_samples_read) {
        fseek(decoder->file, decoder->audio_data_begin, SEEK_SET);
        bit_stream_init(decoder->bit_stream);

        /* ブロックを初期化 */
        decoder->current_read_sub_block_channel = 0;
        decoder->current_read_sub_block_offset = 0;
        decoder->num_samples_read = 0;

        /* 各種フィルタの初期化を行う */
        for (ch = 0; ch < decoder->num_channels; ++ch) {
            lms_clear(decoder->lms_filters[ch]);
            polynomial_predictor_clear(decoder->polynomial_predictors[ch]);
        }
        offset = 0;
    }
    else {
        offset = decoder->num_samples_read;
    }

    /* 指定されたサンプルのインデックスまで読み飛ばす */
    for (; offset < sample_offset; ++offset) {
        force_read_sample(decoder);
    }

    /* シーク中フラグを折る */
    decoder->is_seeking = false;
}

/*!
 * @brief                   ミリ秒単位で指定された時間までシークします。
 * @param *decoder          デコーダのハンドル
 * @param sample_offset     シーク先時間（ミリ秒）
 */
void decoder_seek_milliseconds_to(decoder* decoder, uint32_t ms) {
    uint32_t samples_per_ms = (decoder->sample_rate * decoder->num_channels) / 1000;
    uint32_t offset = ms * samples_per_ms;

    decoder_seek_sample_to(decoder, offset);
}

/*!
 * @brief                   デコーダが開いているファイルの演奏時間をミリ秒単位で取得します。
 * @param *decoder          デコーダのハンドル
 * @return                  演奏時間（ミリ秒単位）
 */
uint32_t decoder_get_duration_ms(decoder* decoder) {
    uint32_t samples_per_ms = (decoder->sample_rate * decoder->num_channels) / 1000;
    uint32_t total_ms = decoder->num_total_samples / samples_per_ms;

    return total_ms;
}