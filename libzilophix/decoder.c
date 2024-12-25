#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/decoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"

const static uint8_t supported_format_versions[2] = { 0x01, 0x10 };

/*! 
 * @brief           �w�肳�ꂽ�o�[�W�����ԍ����A���̃f�R�[�_�ŃT�|�[�g����Ă���t�H�[�}�b�g�o�[�W�����ł��邩�𔻒肵�܂��B
 * @param version   �o�[�W�����ԍ�
 * @return          �T�|�[�g����Ă���o�[�W�����Ȃ� true ���A�����łȂ���� false ��Ԃ��܂�
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
 * @brief           �J����Ă���t�@�C����ZilophiX�t�H�[�}�b�g�ł��邩�ǂ������A�}�W�b�N�i���o�[���ȂĊm�F���܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
 * @return          �J����Ă���t�@�C����ZilophiX�t�H�[�}�b�g�ł����true���A����ȊO�̃t�H�[�}�b�g�ł����false��Ԃ��܂��B
 */
static bool check_magic_number(decoder* decoder){
    return  read_uint8(decoder->file) == 0x5A &&
            read_uint8(decoder->file) == 0x70 &&
            read_uint8(decoder->file) == 0x58;
}

/*!
 * @brief           �t�H�[�}�b�g����ǂݍ��݂܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
 */
static void read_format_info(decoder* decoder){
    /* �t�H�[�}�b�g�o�[�W������ǂݍ��� */
    decoder->format_version = read_uint8(decoder->file);

    if (!is_supported_version(decoder->format_version)) {
        report_error(ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION);
        return;
    }

    /* PCM�t�H�[�}�b�g����ǂݍ��� */
    decoder->sample_rate = read_uint32(decoder->file);
    decoder->bits_per_sample = read_uint8(decoder->file);
    decoder->num_channels = read_uint8(decoder->file);
    decoder->num_total_samples = read_uint32(decoder->file);

    /* Zilophix�G���R�[�h����ǂݍ��� */
    decoder->filter_taps = read_uint8(decoder->file);
    decoder->block_size = read_uint16(decoder->file);
    decoder->use_mid_side_stereo = read_bool(decoder->file);
    decoder->num_blocks = read_uint32(decoder->file);

    /* �\��ς݃R�[�h��ǂݍ��ށi���g�p�Ȃ����������Ȃ�[���j */
    decoder->reserved1 = read_uint8(decoder->file);
    decoder->reserved2 = read_uint8(decoder->file);

    /* �I�[�f�B�I�f�[�^�̊J�n�I�t�Z�b�g��ǂݍ��� */
    if (read_char(decoder->file) == 'A' && read_char(decoder->file) == 'D' && read_char(decoder->file) == 'D' && read_char(decoder->file) == 'R'){
        decoder->audio_data_begin = read_uint32(decoder->file);
    }

    /* �I�[�f�B�I�f�[�^�̃T�C�Y��ǂݍ��� */
    if (read_char(decoder->file) == 'S' && read_char(decoder->file) == 'I' && read_char(decoder->file) == 'Z' && read_char(decoder->file) == 'E'){
        decoder->audio_data_size = read_uint32(decoder->file);
    }
}

/*!
 * @brief           �w�肳�ꂽ�n���h���̃f�R�[�_�ŊJ���ꂽ�t�@�C������AZilophix�t�@�C���̃w�b�_����ǂݍ��݂܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
 */
static void read_header(decoder* decoder) {
    if (check_magic_number(decoder)){
        /* �t�H�[�}�b�g����ǂݍ��� */
        read_format_info(decoder);

        /* �^�O����ǂݍ��� */
        tag_read(decoder->file, &decoder->tag);
    }
    else {
        /* �}�W�b�N�i���o�[���s���Ȓl�ł���|�̃G���[�����|�[�g���� */
        report_error(ERROR_DECODER_INVALID_MAGIC_NUMBER);
    }
}

/*!
 * @brief           ���m�����̃u���b�N��A�~�b�h�T�C�h�X�e���I���g�p����Ă��Ȃ��u���b�N���f�R�[�h���܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
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
            /* STEP 1. SSLMS�t�B���^��K�p���A�������\����̗\���c���𕜌� */
            residual = sb->samples[offset];
            sample = residual + lms_predict(lms);
            lms_update(lms, sample, residual);

            /* STEP 2. �������\����ŗ\�����ꂽ�M���ɗ\���c�������Z���A���̐M���𕜌� */
            sample += polynomial_predictor_predict(poly);
            polynomial_predictor_update(poly, sample);

            /* STEP 3. �������ꂽ�M�����T�u�u���b�N�Ɋi�[ */
            sb->samples[offset] = sample;
        }
    }
}

/*!
 * @brief           �X�e���I�ŁA�~�b�h�T�C�h�X�e���I���g�p����Ă���u���b�N���f�R�[�h���܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
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
        /* STEP 1.  SSLMS�t�B���^��K�p���A���E�`�����l���̑������\����̗\���c���𕜌� */
        mid = lch->samples[offset] + lms_predict(llms);
        side = rch->samples[offset] + lms_predict(rlms);
        lms_update(llms, mid, lch->samples[offset]);
        lms_update(rlms, side, rch->samples[offset]);

        /* STEP 2. �������\����ŗ\�����ꂽ�M���ɗ\���c�������Z���A���̐M���𕜌� */
        mid += polynomial_predictor_predict(lpoly);
        side += polynomial_predictor_predict(rpoly);
        polynomial_predictor_update(lpoly, mid);
        polynomial_predictor_update(rpoly, side);

        /* STEP 3. �~�b�h�T�C�h�X�e���I��LR�X�e���I�ɕϊ����� */
        mid = LSHIFT(mid, 1) | (side & 1);
        left = RSHIFT(mid + side, 1);
        right = RSHIFT(mid - side, 1);

        /* STEP 4. �f�R�[�h���ꂽ�T���v�����T�u�u���b�N�Ɋi�[ */
        lch->samples[offset] = left;
        rch->samples[offset] = right;
    }
}

/*!
 * @brief           �w�肳�ꂽ�f�R�[�_�œǂݍ��ݍς݂̃u���b�N�̃f�R�[�h���s���܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
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
 * @brief           �w�肳�ꂽ�f�R�[�_���A�w�肳�ꂽ�p�X�̃t�@�C�����f�R�[�h�ł���悤�ɏ��������܂��B
 * @param *decoder  �f�R�[�_�̃n���h��
 * @param *path     �t�@�C���̃p�X
 */
static void decoder_init(decoder* decoder, FILE* file) {
    uint8_t ch;

    /* �r�b�g�X�g���[���̏����� */
    decoder->file = file;
    decoder->bit_stream = bit_stream_create(decoder->file, BIT_STREAM_MODE_READ);

    /* �w�b�_����ǂݍ��� */
    read_header(decoder);

    decoder->lms_filters = (lms**)malloc(sizeof(lms*) * decoder->num_channels);
    decoder->polynomial_predictors = (polynomial_predictor**)malloc(sizeof(polynomial_predictor*) * decoder->num_channels);
    decoder->coder = code_create(decoder->bit_stream, decoder->format_version, decoder->bits_per_sample);
    decoder->current_block = (block*)malloc(sizeof(block));
    decoder->current_read_sub_block_channel = 0;
    decoder->current_read_sub_block_offset = 0;
    decoder->num_samples_read = 0;
    decoder->is_seeking = false;

    /* �u���b�N�������� */
    block_init(decoder->current_block, decoder->block_size, decoder->num_channels);

    /* �̈�̊m�ۂɐ������Ă���΁A���������s�� */
    for (ch = 0; ch < decoder->num_channels; ++ch) {
        decoder->lms_filters[ch] = lms_create(decoder->filter_taps, decoder->bits_per_sample);
        decoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }
}

/*!
 * @brief       �f�R�[�_�̃n���h���𐶐����܂��B
 * @param path  �f�R�[�h����t�@�C���̃n���h��
 * @return      �f�R�[�_�̃n���h��
 */
decoder* decoder_create(FILE* file) {
    decoder* result = (decoder*)malloc(sizeof(decoder));

    /* ���������� */
    decoder_init(result, file);
    
    /* �I�[�f�B�I�f�[�^�̊J�n�ʒu�ɃV�[�N */
    fseek(result->file, result->audio_data_begin, SEEK_SET);
    
    return result;
}

/*!
 * @brief           �f�R�[�_��������܂��B
 * @param decoder   �f�R�[�_�̃n���h��
 */
void decoder_free(decoder* decoder) {
    uint8_t ch;

    /* �e�`�����l���p�̃t�B���^����� */
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
 * @brief           �t�@�C������܂��B
 * @param decoder   �f�R�[�_�̃n���h��
 */
void decoder_close(decoder* decoder) {
    tag_free(decoder->tag);
    fclose(decoder->file);
}

/*!
 * @brief           �����I�Ɏ���1�T���v����ǂݍ��݁APCM�T���v���Ƃ��ĕԂ��܂��B
 * @param decoder   �f�R�[�_�̃n���h��
 * @return          �f�R�[�h���ꂽPCM�T���v��
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
 * @brief           ����1�T���v����ǂݍ��݁APCM�T���v���Ƃ��ĕԂ��܂��B
 * @param decoder   �f�R�[�_�̃n���h��
 * @return          �f�R�[�h���ꂽPCM�T���v��
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
 * @brief                   �w�肳�ꂽ�I�t�Z�b�g�̃T���v���܂ŃV�[�N���܂��B
 * @param *decoder          �f�R�[�_�̃n���h��
 * @param sample_offset     �V�[�N��̃T���v���̃I�t�Z�b�g
 */
void decoder_seek_sample_to(decoder* decoder, uint32_t sample_offset) {
    uint8_t ch;
    uint32_t offset;

    /* �V�[�N���t���O�𗧂Ă� */
    decoder->is_seeking = true;

    /* ����V�[�N�i�Đ��ʒu���ߋ��ɖ߂��j�̏ꍇ�A�t�@�C���̍ŏ�����f�R�[�h����蒼���B*/
    if (sample_offset < decoder->num_samples_read) {
        fseek(decoder->file, decoder->audio_data_begin, SEEK_SET);
        bit_stream_init(decoder->bit_stream);

        /* �u���b�N�������� */
        decoder->current_read_sub_block_channel = 0;
        decoder->current_read_sub_block_offset = 0;
        decoder->num_samples_read = 0;

        /* �e��t�B���^�̏��������s�� */
        for (ch = 0; ch < decoder->num_channels; ++ch) {
            lms_clear(decoder->lms_filters[ch]);
            polynomial_predictor_clear(decoder->polynomial_predictors[ch]);
        }
        offset = 0;
    }
    else {
        offset = decoder->num_samples_read;
    }

    /* �w�肳�ꂽ�T���v���̃C���f�b�N�X�܂œǂݔ�΂� */
    for (; offset < sample_offset; ++offset) {
        force_read_sample(decoder);
    }

    /* �V�[�N���t���O��܂� */
    decoder->is_seeking = false;
}

/*!
 * @brief                   �~���b�P�ʂŎw�肳�ꂽ���Ԃ܂ŃV�[�N���܂��B
 * @param *decoder          �f�R�[�_�̃n���h��
 * @param sample_offset     �V�[�N�掞�ԁi�~���b�j
 */
void decoder_seek_milliseconds_to(decoder* decoder, uint32_t ms) {
    uint32_t samples_per_ms = (decoder->sample_rate * decoder->num_channels) / 1000;
    uint32_t offset = ms * samples_per_ms;

    decoder_seek_sample_to(decoder, offset);
}

/*!
 * @brief                   �f�R�[�_���J���Ă���t�@�C���̉��t���Ԃ��~���b�P�ʂŎ擾���܂��B
 * @param *decoder          �f�R�[�_�̃n���h��
 * @return                  ���t���ԁi�~���b�P�ʁj
 */
uint32_t decoder_get_duration_ms(decoder* decoder) {
    uint32_t samples_per_ms = (decoder->sample_rate * decoder->num_channels) / 1000;
    uint32_t total_ms = decoder->num_total_samples / samples_per_ms;

    return total_ms;
}