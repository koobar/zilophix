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

/* MSVC �ɂ� __builtin_clz ���܂܂�Ȃ����߁A���O�Ŏ������� */
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
 * @brief                   �w�肳�ꂽ�f�[�^���w�肳�ꂽ�p�����[�^�Ń��C�X�����������ꍇ�̃r�b�g�����v�Z���܂��B
 * @param *data             �f�[�^
 * @param data_size         �f�[�^�̃T�C�Y
 * @param parameter         ���C�X�������̃p�����[�^
 * @return                  �w�肳�ꂽ�f�[�^���A�w�肳�ꂽ�p�����[�^�Ń��C�X�����������ꍇ�̃r�b�g��
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
 * @brief               �w�肳�ꂽ�f�[�^�ɑ΂��čœK�ƂȂ郉�C�X�������̃p�����[�^���v�Z���܂��B
 * @param *coder        ��������̃n���h��
 * @param *data         �f�[�^�̃|�C���^
 * @param start         �p�[�e�B�V�����̊J�n�I�t�Z�b�g
 * @param data_size     �p�[�e�B�V�����̃T�C�Y
 * @param *total_bits   [�o��]���߂�ꂽ�p�����[�^�Ń��C�X�����������ꍇ�̃f�[�^�̍��v�r�b�g��
 * @return              �w�肳�ꂽ�f�[�^�ɑ΂��čœK�ƂȂ郉�C�X�������̃p�����[�^
 */
static uint32_t compute_optimal_rice_parameter(code* coder, const int32_t* data, uint16_t start, uint16_t data_size, uint32_t* total_bits) {
    double sum = 0;
    double mean = 0;
    uint32_t i, imean, parameter;

    /* �o������f�[�^�̐�Βl�̕��ς����߂�B*/
    for (i = 0; i < data_size; ++i) {
        sum += abs(data[start + i]);
    }
    mean = sum / data_size;

    /* ���ς𐮐��Ɋۂߍ��񂾒l�̕\���ɕK�v�ȃr�b�g�����p�����[�^�Ƃ���B*/
    imean = CLAMP((uint32_t)floor(mean), 0, UINT32_MAX);
    if (imean == 0){
        /* ���ς��[���̏ꍇ�A��]���[���r�b�g�o��(=���o��)�ł�OK */
        parameter = 0;
    }
    else{
        /* ���ς��[���ȊO�̏ꍇ�A�œK�p�����[�^�� (32-���ς̕\���ɕK�v�ȍŏ��r�b�g��) �Ő���\ */
        parameter = CLAMP(32 - __builtin_clz(imean), 0, coder->rice_parameter_max);
    }
    
    *(total_bits) = compute_rice_total_bits(data, start, data_size, parameter);
    return parameter;
}

/*!
 * @brief                   �p�[�e�B�V�����Ŏg�p���ׂ��G���g���s�[�������̃p�����[�^���v�Z���܂��B
 * @param *coder            ��������̃n���h��
 * @param *data             �f�[�^�S�̂̃|�C���^
 * @param start             �p�[�e�B�V�����̊J�n�ʒu
 * @param partition_size    �p�[�e�B�V�����̃T�C�Y
 * @param *partition_bits   [�o��]���߂�ꂽ�G���g���s�[�������̃p�����[�^���g�p���ăG���g���s�[�����������ꍇ�̃p�[�e�B�V�����S�̂̃r�b�g��
 * @return                  �œK�ȃG���g���s�[�������p�����[�^
 */
static uint32_t compute_optimal_entropy_parameter(code* coder, const int32_t* data, uint16_t start, uint16_t partition_size, uint32_t* partition_bits) {
    bool is_blank_partition = true;
    uint16_t offset;

    /* �u�����N�p�[�e�B�V�����i���ׂĂ̒l���[���ł���p�[�e�B�V�����j�ł��邩���肷��B*/
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
 * @brief               �w�肳�ꂽ�f�[�^�ɑ΂��čœK�ƂȂ�p�[�e�B�V�����p�����[�^�A����сA���ꂼ��̃p�[�e�B�V�����Ŏg�p���ׂ��G���g���s�[�������̃p�����[�^���v�Z���܂��B
 * @param *coder        ��������̃n���h��
 * @param *data         �f�[�^�̃|�C���^
 * @param data_size     �f�[�^�̃T�C�Y
 * @return              �w�肳�ꂽ�f�[�^�ɑ΂��čœK�ƂȂ�p�[�e�B�V�����p�����[�^
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

        /* ���ꂼ��̃p�[�e�B�V�����ɂƂ��čœK�ƂȂ�G���g���s�[�������̃p�����[�^�����߁A��Ɨ̈�Ɋi�[���� */
        for (partition_index = 0; partition_index < partition_count; ++partition_index) {
            start = partition_size * partition_index;

            /* ���̃p�[�e�B�V�����̃G���g���s�[�������̃p�����[�^����Ɨ̈�ɕۑ� */
            coder->workB[partition_index] = compute_optimal_entropy_parameter(coder, data, start, partition_size, &partition_bits);
            
            /* �p�[�e�B�V�����̃T�C�Y�����Z */
            size += partition_bits;
        }

        /* �p�[�e�B�V�����̃T�C�Y�ɁA�G���g���s�[�������̃p�����[�^��ۑ����邽�߂ɗv����r�b�g�������Z */
        size += PARTITION_PARAMETER_NEED_BITS;

        /* ���łɔ������ꂽ�p�[�e�B�V�����̍ŏ��T�C�Y���A���񎎂����p�����[�^�ɂ��
         * �p�[�e�B�V�����T�C�Y�̂ق�����������΁A�ŏ��T�C�Y��u�������A��Ɨ̈�Ɋi�[���ꂽ�A
         * ���ׂẴp�[�e�B�V�����̃G���g���s�[�������̃p�����[�^���A�o�͗̈�ɃR�s�[����B*/
        if (min_size > size) {
            min_size = size;
            optimal_partition_parameter = trial_pp;
            memcpy(coder->workA, coder->workB, sizeof(uint32_t) * PARTITION_COUNT_MAX);
        }
    }

    return optimal_partition_parameter;
}

/*!
 * @brief                   Rice������p���ăp�[�e�B�V�����Ɋ܂܂��\���c�����r�b�g�X�g���[���ɏ������݂܂��B
 * @param *stream           �r�b�g�X�g���[���̃n���h��
 * @param *data             �������ޗ\���c���̃|�C���^
 * @param start             �p�[�e�B�V�����̊J�n�I�t�Z�b�g
 * @param partition_size    �p�[�e�B�V�����̃T�C�Y
 * @param parameter         ���C�X�������̃p�����[�^
 */
static void write_rice_partition(bit_stream* stream, const int32_t* data, uint16_t start, uint16_t partition_size, uint32_t parameter) {
    uint16_t offset;

    for (offset = 0; offset < partition_size; ++offset) {
        bit_stream_write_signed_rice(stream, data[offset + start], parameter);
    }
}

/*!
 * @brief                   Rice�������g�p���ꂽ�p�[�e�B�V�����Ɋ܂܂��\���c����ǂݍ��݂܂��B
 * @param *stream           �r�b�g�X�g���[���̃n���h��
 * @param *data             �ǂݍ��܂ꂽ�\���c�����i�[����̈�̃|�C���^
 * @param start             �p�[�e�B�V�����̊J�n�I�t�Z�b�g
 * @param partition_size    �p�[�e�B�V�����̃T�C�Y
 * @param parameter         ���C�X�������̃p�����[�^
 */
static void read_rice_partition(bit_stream* stream, int32_t* data, uint16_t start, uint16_t partition_size, uint32_t parameter) {
    uint16_t offset;
    int32_t* pdata = data + start;

    for (offset = 0; offset < partition_size; ++offset) {
        *(pdata++) = bit_stream_read_rice_signed(stream, parameter);
    }
}

/*!
 * @brief               �w�肳�ꂽ�r�b�g�X�g���[���ɁA�w�肳�ꂽ�T�u�u���b�N�����C�X���������ď������݂܂��B
 * @param *coder        ��������̃n���h��
 * @param *sub_block    �T�u�u���b�N�̃n���h��
 */
static void write_sub_block(code* coder, const sub_block* sub_block) {
    uint32_t p, start;
    uint32_t parameter;
    uint16_t partition_size;
    uint32_t partition_parameter;
    uint32_t partition_count;

    /* ���C�X�������̃p�[�e�B�V�����p�����[�^���v�Z */
    partition_parameter = compute_optimal_partition_parameter(coder, sub_block->samples, sub_block->size);

    /* �p�[�e�B�V�����p�����[�^����p�[�e�B�V�������ƃp�[�e�B�V�����̃T�C�Y���v�Z */
    partition_count = RESTORE_PARTITION_COUNT(partition_parameter);
    partition_size = sub_block->size / partition_count;

    /* �p�[�e�B�V�����p�����[�^��ۑ����� */
    bit_stream_write_uint(coder->bitstream, partition_parameter - PARTITION_PARAMETER_MIN, PARTITION_PARAMETER_NEED_BITS);

    /* �T�u�u���b�N���G���g���s�[���������ď������� */
    for (p = 0; p < partition_count; ++p) {
        start = partition_size * p;
        parameter = coder->workA[p];

        /* �G���g���s�[�������̃p�����[�^���������� */
        bit_stream_write_uint(coder->bitstream, (uint32_t)(parameter), coder->bits_of_entropy_parameter);

        /* �G���g���s�[�����̃p�����[�^��Rice�����͈͓̔��Ȃ�ARice�����Ńp�[�e�B�V�������������� */
        if (parameter <= coder->rice_parameter_max) {
            write_rice_partition(coder->bitstream, sub_block->samples, start, partition_size, parameter);
        }
    }
}

/*!
 * @brief               �w�肳�ꂽ�r�b�g�X�g���[������A���C�X����������ď������܂ꂽ�T�u�u���b�N�̃f�[�^��ǂݍ��݁A�w�肳�ꂽ�T�u�u���b�N�Ɋi�[���܂��B
 * @param *coder        ��������̃n���h��
 * @param *sub_block    �T�u�u���b�N�̃n���h��
 */
static void read_sub_block(code* coder, sub_block* sub_block) {
    uint32_t parameter;
    uint16_t offset;
    uint32_t p, start;
    uint16_t partition_size;
    uint32_t partition_parameter;
    uint32_t partition_count;

    /* �p�[�e�B�V�����p�����[�^���擾 */
    partition_parameter = bit_stream_read_uint(coder->bitstream, PARTITION_PARAMETER_NEED_BITS) + PARTITION_PARAMETER_MIN;

    /* ���C�X�������̃p�[�e�B�V�������ƃp�[�e�B�V�����T�C�Y���v�Z */
    partition_count = RESTORE_PARTITION_COUNT(partition_parameter);
    partition_size = sub_block->size / partition_count;

    /* �G���g���s�[���������ꂽ������ǂݍ��� */
    for (p = 0; p < partition_count; ++p) {
        start = p * partition_size;

        /* �G���g���s�[�������̃p�����[�^���擾 */
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
 * @brief               �u���b�N��ǂݏ�������API�̃n���h���𐶐����܂��B
 * @param *stream       �r�b�g�X�g���[���̃n���h��
 * @param fmt_version   �t�H�[�}�b�g�̃o�[�W����
 * @return              �u���b�N�ǂݏ���API�̃n���h��
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
        /* 16�r�b�gPCM�f�[�^�������ɂ�4�r�b�g�͈͂̃G���g���s�[�������A24�r�b�gPCM�f�[�^�������ɂ�5�r�b�g�͈͂̃G���g���s�[�������p�����[�^��p���� */
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
 * @brief           �u���b�N��ǂݏ�������API�̃n���h����������܂��B
 * @param *coder    �u���b�N�ǂݏ���API�̃n���h��
 */
void code_free(code* coder) {
    free(coder->workA);
    free(coder->workB);
}

/*!
 * @brief           �u���b�N���������݂܂��B
 * @param *coder    �u���b�N�ǂݏ���API�̃n���h��
 * @param *block    �������ރu���b�N�̃n���h��
 */
void code_write_block(code* coder, const block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        write_sub_block(coder, block->sub_blocks[ch]);
    }
}

/*!
 * @brief           �u���b�N��ǂݍ��݂܂��B
 * @param *coder    �u���b�N�ǂݏ���API�̃n���h��
 * @param *block    �ǂݍ��񂾃f�[�^���i�[����u���b�N�̃n���h��
 */
void code_read_block(code* coder, block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        read_sub_block(coder, block->sub_blocks[ch]);
    }
}