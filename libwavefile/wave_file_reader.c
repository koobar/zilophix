#include "./include/wave_file_reader.h"
#include <stdbool.h>
#include <stdlib.h>

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������8�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
static uint8_t read_uint8(FILE* file) {
    uint8_t value;
    fread(&value, sizeof(uint8_t), 1, file);

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������16�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
static uint16_t read_uint16(FILE* file) {
    uint16_t value;
    fread(&value, sizeof(uint16_t), 1, file);

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������32�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
static uint32_t read_uint32(FILE* file) {
    uint32_t value;
    fread(&value, sizeof(uint32_t), 1, file);

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������16�r�b�g�����t��������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
static int16_t read_int16(FILE* file) {
    int16_t value;
    fread(&value, sizeof(int16_t), 1, file);

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������ASCII������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
static char read_char(FILE* file) {
    char value;
    fread(&value, sizeof(char), 1, file);

    return value;
}

/*!
 * @brief           �w�肳�ꂽ�n���h����wave_file_reader���J���Ă���t�@�C���̓ǂݍ��݈ʒu���A�w�肳�ꂽASCII�����Ɉ�v����o�C�g�񂪏o������ӏ��Ɉړ����܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @param *data     ASCII����
 * @param n         ASCII�����̕�����
 * @return          �w�肳�ꂽASCII�����ɑΉ�����o�C�g�񂪏o���������ǂ����������l
 */
static bool check_match_next_bytes(const wave_file_reader* reader, const char* data, uint32_t n) {
    register uint32_t i;
    register char read;

    for (i = 0; i < n; ++i) {
        read = read_char(reader->wave_file);

        if (read != data[i]) {
            return false;
        }
    }

    return true;
}

/*!
 * @brief                   �w�肳�ꂽ�n���h����wave_file_reader���J���Ă���t�@�C���̓ǂݍ��݈ʒu���A�w�肳�ꂽ�`�����N���̃`�����N�̊J�n�ʒu�Ɉړ����܂��B
 * @param *reader           wave_file_reader�̃n���h��
 * @param *chunk_name       �`�����N��
 * @param n                 �`�����N���̕�����
 * @param find_from_begin   �t�@�C���̍ŏ�����`�����N�̒T�����s�����ǂ����������t���O
 * @return                  �w�肳�ꂽ���O�̃`�����N�������������ǂ����������l
 */
static bool go_to_chunk(const wave_file_reader* reader, const char* chunk_name, uint32_t n, bool find_from_begin) {
    if (find_from_begin) {
        fseek(reader->wave_file, 0, SEEK_SET);
    }

    for(;;) {
        if (check_match_next_bytes(reader, chunk_name, n)) {
            return true;
        }
    }

    return false;
}

/*!
 * @brief                   �w�肳�ꂽ�n���h����wave_file_reader���J���Ă���t�@�C������fmt�`�����N��ǂݍ��݂܂��B
 */
static void read_fmt_chunk(wave_file_reader* reader) {
    uint16_t audio_format;

    if (go_to_chunk(reader, "fmt ", 4, true)) {
        read_uint32(reader->wave_file);                                     /* �`�����N�T�C�Y��ǂݔ�΂� */
        audio_format = read_uint16(reader->wave_file);

        /* �I�[�f�B�I�t�H�[�}�b�g��PCM���H */
        if (audio_format == 0x0001 || audio_format == 0xFFFE) {
            reader->num_channels = read_uint16(reader->wave_file);          /* �`�����l���� */
            reader->sample_rate = read_uint32(reader->wave_file);           /* �T���v�����O���g�� */
            reader->avr_bytes_per_sec = read_uint32(reader->wave_file);     /* 1�b������̕��σo�C�g�� */
            reader->block_size = read_uint16(reader->wave_file);            /* �u���b�N�T�C�Y */
            reader->bits_per_sample = read_uint16(reader->wave_file);       /* �ʎq���r�b�g�� */
        }
    }
}

/*!
 * @brief           wave_file_reader�̃n���h���𐶐����܂��B
 * @path            �ǂݍ���WAV�t�@�C���̃p�X
 * @return          wave_file_reader�̃n���h��
 */
wave_file_reader* wave_file_reader_create(const char* path) {
    wave_file_reader* result = (wave_file_reader*)malloc(sizeof(wave_file_reader));
    
    if (result == NULL) {
        return NULL;
    }

    wave_file_reader_open(result, path);
    return result;
}

/*!
 * @brief           WAV�t�@�C�����J���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @param *path     �t�@�C���p�X
 */
void wave_file_reader_open(wave_file_reader* reader, const char* path) {
    uint32_t size;

    /* �t�@�C�����o�C�i���ǂݍ��݃��[�h�ŊJ�� */
    reader->wave_file = fopen(path, "rb");

    /* fmt �`�����N��ǂݍ��� */
    read_fmt_chunk(reader);

    /* data�`�����N�Ɉړ� */
    if (go_to_chunk(reader, "data", 4, true)) {
        /* ���v�T���v�������v�Z */
        size = read_uint32(reader->wave_file);
        reader->num_samples = size / (reader->bits_per_sample / 8);
    }
    else {
        reader->num_samples = 0;
    }
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C������܂��B
 * @param *reader   wave_file_reader�̃n���h��
 */
void wave_file_reader_close(const wave_file_reader* reader) {
    fclose(reader->wave_file);
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̃T���v�����O���g�����擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �T���v�����O���g��
 */
uint32_t wave_file_reader_get_sample_rate(const wave_file_reader* reader) {
    return reader->sample_rate;
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̗ʎq���r�b�g�����擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �ʎq���r�b�g��
 */
uint16_t wave_file_reader_get_bits_per_sample(const wave_file_reader* reader) {
    return reader->bits_per_sample;
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̃`�����l�������擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �`�����l����
 */
uint16_t wave_file_reader_get_num_channels(const wave_file_reader* reader) {
    return reader->num_channels;
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̑��T���v�������擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          ���T���v����
 */
uint32_t wave_file_reader_get_num_samples(const wave_file_reader* reader) {
    return reader->num_samples;
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C�����玟�̃T���v����ǂݍ��݂܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �T���v��
 */
int32_t wave_file_reader_read_sample(wave_file_reader* reader) {
    register int32_t result = 0;
    register uint8_t sb1, sb2;
    register int8_t sb3;

    switch (reader->bits_per_sample) {
    case 8:
        result = (int32_t)read_uint8(reader->wave_file) - 128;
        break;
    case 16:
        result = read_int16(reader->wave_file);
        break;
    case 24:
        sb1 = read_uint8(reader->wave_file);
        sb2 = read_uint8(reader->wave_file);
        sb3 = read_uint8(reader->wave_file);
        result = (sb3 << 16) | (sb2 << 8) | sb1;
        break;
    default:
        break;
    }

    reader->num_samples_read++;
    return result;
}