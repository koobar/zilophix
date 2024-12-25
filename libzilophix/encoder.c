#include "./include/file_access.h"
#include "./include/macro.h"
#include "./include/zilophix.h"
#include "./include/encoder.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include <stdlib.h>

/*!
 * @brief           �}�W�b�N�i���o�[���������݂܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
*/
static void write_magic_number(encoder* encoder){
    /* �}�W�b�N�i���o�[���������ށB */
    write_uint8(encoder->output_file, 0x5A);
    write_uint8(encoder->output_file, 0x70);
    write_uint8(encoder->output_file, 0x58);
}

/*!
 * @brief           �t�H�[�}�b�g�����������݂܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
*/
static void write_format_info(encoder* encoder){
    /* �t�H�[�}�b�g�̃o�[�W�������������ށB */
    write_uint8(encoder->output_file, FORMAT_VERSION_CURRENT);

    /* PCM�t�H�[�}�b�g�����������ށB */
    write_uint32(encoder->output_file, encoder->sample_rate);
    write_uint8(encoder->output_file, encoder->bits_per_sample);
    write_uint8(encoder->output_file, encoder->num_channels);
    write_uint32(encoder->output_file, encoder->num_samples);

    /* �G���R�[�h��� */
    write_uint8(encoder->output_file, encoder->filter_taps);
    write_uint16(encoder->output_file, encoder->block_size);
    write_bool(encoder->output_file, encoder->use_mid_side_stereo);
    write_uint32(encoder->output_file, encoder->num_blocks);

    /* �\��ς݃R�[�h���������ށi�������͏�Ƀ[���j*/
    write_uint8(encoder->output_file, 0x00);
    write_uint8(encoder->output_file, 0x00);

    /* �I�[�f�B�I�f�[�^�̊J�n�A�h���X���������ޗ̈��ێ����A�_�~�[�A�h���X����������
     * �I�[�f�B�I�f�[�^�̊J�n�ʒu�𖾊m�ɋL�^���邱�ƂŁA�Ⴆ�o�[�W�����A�b�v�Ȃǂ�
     * ����ȍ~�̃f�[�^�\�����ω����Ă��A�����܂ł���������΁A�ǂ����炪���k���ꂽ
     * �I�[�f�B�I�f�[�^�ł���̂����ʂ��邱�Ƃ��ł���B*/
    write_char(encoder->output_file, 'A');
    write_char(encoder->output_file, 'D');
    write_char(encoder->output_file, 'D');
    write_char(encoder->output_file, 'R');
    encoder->audio_data_position_position = (uint32_t)ftell(encoder->output_file);
    write_uint32(encoder->output_file, 0xFFFFFFFF);

    /* �I�[�f�B�I�f�[�^�̃T�C�Y���������ޗ̈��ێ����A�_�~�[�T�C�Y����������
     * �I�[�f�B�I�f�[�^�̃T�C�Y�𖾊m�ɋL�^���邱�ƂŁA�Ⴆ�o�[�W�����A�b�v�Ȃǂ�
     * ����ȍ~�̃f�[�^�\�����ω����Ă��A�����܂ł���������΁A���k���ꂽ
     * �I�[�f�B�I�f�[�^�̃T�C�Y�𔻕ʂ��邱�Ƃ��ł���B*/
    write_char(encoder->output_file, 'S');
    write_char(encoder->output_file, 'I');
    write_char(encoder->output_file, 'Z');
    write_char(encoder->output_file, 'E');
    encoder->audio_data_size_position = (uint32_t)ftell(encoder->output_file);
    write_uint32(encoder->output_file, 0xFFFFFFFF);
}

/*!
 * @brief           �w�b�_���̏������݂��s���܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
 */
static void write_header(encoder* encoder) {
    /* �}�W�b�N�i���o�[���������ށB */
    write_magic_number(encoder);

    /* �t�H�[�}�b�g������������ */
    write_format_info(encoder);
    
    /* �^�O������������ */
    tag_write(encoder->output_file, encoder->tag);

    /* �f�[�^�̊J�n�I�t�Z�b�g��ێ� */
    encoder->audio_data_begin_position = (uint32_t)ftell(encoder->output_file);
}

/*!
 * @brief           �I�[�f�B�I�f�[�^�����̏������݂��I�����A�f�[�^�̊J�n�A�h���X�ƃT�C�Y���������݂܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
*/
static void flush_audio_data(encoder* encoder){
    uint32_t current_len = (uint32_t)ftell(encoder->output_file);
    uint32_t data_size = current_len - (encoder->audio_data_begin_position);

    /* �I�[�f�B�I�f�[�^�̊J�n�A�h���X�̃A�h���X�Ɉړ����A�J�n�A�h���X���������� */
    fseek(encoder->output_file, encoder->audio_data_position_position, SEEK_SET);
    write_uint32(encoder->output_file, encoder->audio_data_begin_position);

    /* �I�[�f�B�I�f�[�^�̃T�C�Y���������ވʒu�Ɉړ����A�T�C�Y���������� */
    fseek(encoder->output_file, encoder->audio_data_size_position, SEEK_SET);
    write_uint32(encoder->output_file, data_size);

    /* ���̈ʒu�Ɉړ� */
    fseek(encoder->output_file, current_len, SEEK_SET);
}

/*!
 * @brief           �u���b�N���G���R�[�h���܂��B�~�b�h�T�C�h�X�e���I���g�p����Ă���ꍇ�Aencode_current_midside_block�֐����g�p���Ă��������B
 * @param *encoder  �G���R�[�_�̃n���h��
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
            /* STEP 1. �������\�� */
            residual = sb->samples[offset] - polynomial_predictor_predict(poly);
            polynomial_predictor_update(poly, sb->samples[offset]);

            /* STEP 2. SSLMS�t�B���^�ɂ��\�� */
            sample = residual;
            residual -= lms_predict(lms);
            lms_update(lms, sample, residual);

            /* STEP 3. �o�� */
            sb->samples[offset] = residual;
        }
    }
}

/*!
 * @brief           �~�b�h�T�C�h�X�e���I�u���b�N���G���R�[�h���܂��B�~�b�h�T�C�h�X�e���I�ł͂Ȃ��ꍇ�Aencode_current_normal_block�֐����g�p���Ă��������B
 * @param *encoder  �G���R�[�_�̃n���h��
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

        /* STEP 1. �~�b�h�T�C�h�X�e���I�ւ̕ϊ� */
        lch->samples[offset] = mid = RSHIFT(left + right, 1);
        rch->samples[offset] = side = left - right;

        /* STEP 2. �������\�� */
        mid -= polynomial_predictor_predict(lpoly);
        side -= polynomial_predictor_predict(rpoly);
        polynomial_predictor_update(lpoly, lch->samples[offset]);
        polynomial_predictor_update(rpoly, rch->samples[offset]);
        lch->samples[offset] = mid;
        rch->samples[offset] = side;

        /* STEP 3. SSLMS�t�B���^�ɂ��\�� */
        mid -= lms_predict(llms);
        side -= lms_predict(rlms);
        lms_update(llms, lch->samples[offset], mid);
        lms_update(rlms, rch->samples[offset], side);

        /* STEP 4. �o�� */
        lch->samples[offset] = mid;
        rch->samples[offset] = side;
    }
}

/*!
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�œǂݍ��܂ꂽ�u���b�N�̃G���R�[�h���s���܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
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
 * @brief               �u���b�N�����v�Z���܂�
 * @param num_samples   �T���v����
 * @param num_channels  �`�����l����
 * @param block_size    �T�u�u���b�N�̃T���v����
 * @return              �u���b�N��
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
 * @brief                           �G���R�[�_�����������܂�
 * @param *encoder                  �G���R�[�_�̃n���h��
 * @param *file                     �o�͐�̃t�@�C���n���h��
 * @param sample_rate               �T���v�����O���g��
 * @param bits_per_sample           �ʎq���r�b�g��
 * @param num_channels              �`�����l����
 * @param num_samples               ���v�T���v����
 * @param block_size                �u���b�N�T�C�Y
 * @param use_mid_side_stereo       �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O
 * @param filter_taps               LMS�t�B���^�̃^�b�v��
 * @param *tag                      �^�O���
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

    /* �u���b�N�������� */
    block_init(encoder->current_block, block_size, num_channels);

    /* �e�`�����l���p�̃t�B���^�������� */
    for (ch = 0; ch < num_channels; ++ch) {
        encoder->lms_filters[ch] = lms_create(filter_taps, bits_per_sample);
        encoder->polynomial_predictors[ch] = polynomial_predictor_create();
    }

    /* �w�b�_������������ */
    write_header(encoder);
}

/*!
 * @brief                           �w�肳�ꂽ�ݒ�ŁA�G���R�[�_�̃n���h���𐶐����܂��B
 * @param *file                     �o�͐�̃t�@�C���n���h��
 * @param sample_rate               �T���v�����O���g��
 * @param bits_per_sample           �ʎq���r�b�g��
 * @param num_channels              �`�����l����
 * @param num_samples               ���v�T���v����
 * @param block_size                �u���b�N�T�C�Y
 * @param use_mid_side_stereo       �~�b�h�T�C�h�X�e���I���g�p���邩�ǂ����������t���O
 * @param filter_taps               LMS�t�B���^�̃^�b�v��
 * @param *tag                      �^�O���
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
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_��������܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
 */
void encoder_free(encoder* encoder) {
    uint8_t ch;

    /* �e�`�����l���p�̃t�B���^����� */
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
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�ŁA�w�肳�ꂽ�T���v�����G���R�[�h���܂��B
 * @param *encoder  �G���R�[�_�̃n���h��
 * @param sample    �T���v��
 */
void encoder_write_sample(encoder* encoder, int32_t sample) {
    int32_t flg_encode_block = 0;

    encoder->current_block->sub_blocks[encoder->current_sub_block_channel++]->samples[encoder->current_sub_block_offset] = sample;

    /* �I�t�Z�b�g�X�V */
    if (encoder->current_sub_block_channel >= encoder->num_channels) {
        encoder->current_sub_block_channel = 0;
        ++encoder->current_sub_block_offset;

        /* �T�u�u���b�N�̃I�t�Z�b�g���u���b�N�T�C�Y�ȏ�ƂȂ����ۂ́A�T�u�u���b�N�̃I�t�Z�b�g���[���ɖ߂��A�G���R�[�h���ꂽ�u���b�N���o�̓X�g���[���ɏ������� */
        if (encoder->current_sub_block_offset >= encoder->block_size) {
            encoder->current_sub_block_offset = 0;
            flg_encode_block = 1;
        }
    }

    /* �o�̓X�g���[���ւ̏������݃t���O�������Ă���Ώ������� */
    if (flg_encode_block == 1) {
        encode_current_block(encoder);
        code_write_block(encoder->coder, encoder->current_block);
    }
}

/*!
 * @brief           �w�肳�ꂽ�n���h���̃G���R�[�_�ł̃G���R�[�h�̏I���������s���܂��B���ׂẴT���v���̃G���R�[�h���I��������ɁA�K���Ăяo���Ă��������B
 * @param *encoder  �G���R�[�_�̃n���h��
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