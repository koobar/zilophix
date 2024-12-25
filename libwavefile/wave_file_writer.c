#include "./include/wave_file_writer.h"
#include <stdlib.h>

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����ASCII�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
static void write_char(FILE* file, const char value) {
    fwrite(&value, sizeof(char), 1, file);
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����16�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
static void write_uint16(FILE* file, const uint16_t value) {
    fwrite(&value, sizeof(uint16_t), 1, file);
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����32�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
static void write_uint32(FILE* file, const uint32_t value) {
    fwrite(&value, sizeof(uint32_t), 1, file);
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����16�����t���r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
static void write_int16(FILE* file, const int16_t value) {
    fwrite(&value, sizeof(int16_t), 1, file);
}

/*!
 * @brief           �w�肳�ꂽ�n���h����wave_file_writer���ێ�����t�H�[�}�b�g���Ɋ�Â����`�����N�T�C�Y���v�Z���܂��B
 * @param *writer   wave_file_reader�̃n���h��
 * @return          �`�����N�T�C�Y
 */
static uint32_t compute_chunk_size(const wave_file_writer* writer) {
    uint32_t size_of_fmt_chunk = 24;
    uint32_t bytes_per_sample = writer->bits_per_sample / 8;
    uint32_t size_of_data_chunk = 4 + writer->num_samples * bytes_per_sample;
    uint32_t chunk_size = size_of_data_chunk + size_of_fmt_chunk;

    return chunk_size;
}

/*!
 * @brief           �w�肳�ꂽ�n���h����wave_file_writer�ŁAWAVE�w�b�_���̏������݂��s���܂��B
 * @param *writer   wave_file_reader�̃n���h��
 */
static void write_wave_header(const wave_file_writer* writer) {
    /* RIFF�̃}�W�b�N�i���o�[���������ށB */
    write_char(writer->wave_file, 0x52);
    write_char(writer->wave_file, 0x49);
    write_char(writer->wave_file, 0x46);
    write_char(writer->wave_file, 0x46);

    /* �`�����N�T�C�Y�����߂ď������ށB */
    write_uint32(writer->wave_file, compute_chunk_size(writer));

    /* 'WAVE' ��ASCII�R�[�h�ŏ������ށB */
    write_char(writer->wave_file, 0x57);
    write_char(writer->wave_file, 0x41);
    write_char(writer->wave_file, 0x56);
    write_char(writer->wave_file, 0x45);
}

/*!
 * @brief           �w�肳�ꂽ�n���h����wave_file_writer�ŁAfmt�`�����N�̏������݂��s���܂��B
 * @param *writer   wave_file_reader�̃n���h��
 */
static void write_fmt_chunk(const wave_file_writer* writer) {
    uint32_t avr_bytes_per_sec;
    uint16_t block_size;

    /* fmt �`�����N�̃w�b�_���������ށB */
    write_char(writer->wave_file, 0x66);
    write_char(writer->wave_file, 0x6d);
    write_char(writer->wave_file, 0x74);
    write_char(writer->wave_file, 0x20);

    /* �`�����N�̃o�C�g�����������ށB */
    write_uint32(writer->wave_file, 16);

    /* �I�[�f�B�I�t�H�[�}�b�g���������ށB */
    write_uint16(writer->wave_file, 0x0001);

    /* �`�����l�������������ށB */
    write_uint16(writer->wave_file, writer->num_channels);

    /* �T���v�����[�g���������ށB */
    write_uint32(writer->wave_file, writer->sample_rate);

    /* 1�b������̕��σo�C�g�����������ށB */
    avr_bytes_per_sec = writer->sample_rate * (writer->bits_per_sample / 8) * writer->num_channels;
    write_uint32(writer->wave_file, avr_bytes_per_sec);

    /* �u���b�N�̃T�C�Y���������ށB */
    block_size = writer->num_channels * writer->bits_per_sample / 8;
    write_uint16(writer->wave_file, block_size);

    /* �ʎq���r�b�g�����������ށB */
    write_uint16(writer->wave_file, writer->bits_per_sample);
}

/*!
 * @brief           �w�肳�ꂽ�n���h����wave_file_writer�ŁAdata�`�����N�̏������݂��s���܂��B
 * @param *writer   wave_file_reader�̃n���h��
 */
static void write_data_chunk_header(const wave_file_writer* writer) {
    uint32_t bytes_per_sample, data_chunk_size;

    /* 'data' ��ASCII�R�[�h�ŏ������ށB */
    write_char(writer->wave_file, 0x64);
    write_char(writer->wave_file, 0x61);
    write_char(writer->wave_file, 0x74);
    write_char(writer->wave_file, 0x61);

    /* �`�����N�T�C�Y���������ށB */
    bytes_per_sample = writer->bits_per_sample / 8;
    data_chunk_size = writer->num_samples * bytes_per_sample;
    write_uint32(writer->wave_file, data_chunk_size);
}

/*!
 * @brief           wave_file_writer�̃n���h���𐶐����܂��B
 * @param *path     �o�͂���WAV�t�@�C���̃p�X
 * @return          wave_file_writer�̃n���h��
 */
wave_file_writer* wave_file_writer_create(const char* path) {
    wave_file_writer* result = (wave_file_writer*)malloc(sizeof(wave_file_writer));

    if (result == NULL) {
        return NULL;
    }

    wave_file_writer_open(result, path);
    return result;
}

/*!
 * @brief           WAV�t�@�C�����쐬���A�w�肳�ꂽ�n���h���ŊJ���܂��B
 * @param *writer   wave_file_writer�̃n���h��
 * @param *path     WAV�t�@�C�����쐬����p�X
 */
void wave_file_writer_open(wave_file_writer* writer, const char* path) {
    writer->wave_file = fopen(path, "wb");
    writer->sample_rate = 0;
    writer->bits_per_sample = 0;
    writer->num_channels = 0;
    writer->num_samples = 0;
    writer->flag_file_header_write = false;
    writer->flag_fmt_chunk_write = false;
    writer->flag_data_chunk_header_write = false;
}

/*!
 * @brief           �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C������܂��B
 * @param *writer   wave_file_writer�̃n���h��
 */
void wave_file_writer_close(const wave_file_writer* writer) {
    fclose(writer->wave_file);
    free(writer->wave_file);
}

/*!
 * @brief                   �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C����PCM�̃t�H�[�}�b�g��ݒ肵�܂��B
 * @param *writer           wave_file_writer�̃n���h��
 * @param sample_rate       �T���v�����O���g��
 * @param bits_per_sample   �ʎq���r�b�g��
 * @param num_channels      �`�����l����
 */
void wave_file_writer_set_pcm_format(wave_file_writer* writer, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t num_channels) {
    writer->sample_rate = sample_rate;
    writer->bits_per_sample = bits_per_sample;
    writer->num_channels = num_channels;
}

/*!
 * @brief                   �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C���̑��T���v������ݒ肵�܂��B
 * @param *writer           wave_file_writer�̃n���h��
 * @param num_samples       ���T���v����
 */
void wave_file_writer_set_num_samples(wave_file_writer* writer, uint32_t num_samples) {
    writer->num_samples = num_samples;
}

/*!
 * @brief                   �w�肳�ꂽ�n���h���ō쐬����WAVE�t�@�C���ւ̃T���v���̏������݂��J�n���܂��B�ŏ��̃T���v���̏������ݑO�ɁA�K���A���̊֐����Ăяo���Ă��������B
 * @param *writer           wave_file_writer�̃n���h��
 */
void wave_file_writer_begin_write(const wave_file_writer* writer) {
    write_wave_header(writer);
    write_fmt_chunk(writer);
    write_data_chunk_header(writer);
}

/*!
 * @brief                   �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C���փT���v�����������݂܂��B�ŏ��ɂ��̊֐����Ăяo���O�ɁA�K���Awave_file_writer_begin_write �֐����Ăяo���Ă��������B
 * @param *writer           wave_file_writer�̃n���h��
 * @param sample            �������ރT���v��
 */
void wave_file_writer_write_sample(const wave_file_writer* writer, int32_t sample) {
    switch (writer->bits_per_sample)
    {
    case 16:
        write_int16(writer->wave_file, (int16_t)sample);
        break;
    case 24:
        write_char(writer->wave_file, (char)(sample >> 0) & 0xFF);
        write_char(writer->wave_file, (char)(sample >> 8) & 0xFF);
        write_char(writer->wave_file, (char)(sample >> 16) & 0xFF);
        break;
    }
}

/*!
 * @brief                   �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C���ւ̃T���v���̏������݂��I�����܂��B���ׂẴT���v�����������񂾌�ɁA�K�����̊֐����Ăяo���Ă��������B
 * @param *writer           wave_file_reader�̃n���h��
 */
void wave_file_writer_end_write(const wave_file_writer* writer) {
    fflush(writer->wave_file);
}