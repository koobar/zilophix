#ifndef WAVE_FILE_WRITER_HEADER_INCLUDED
#define WAVE_FILE_WRITER_HEADER_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* wave_file;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
    uint32_t num_samples;
    bool flag_file_header_write;
    bool flag_fmt_chunk_write;
    bool flag_data_chunk_header_write;
} wave_file_writer;


/*!
 * @brief           wave_file_writer�̃n���h���𐶐����܂��B
 * @param *path     �o�͂���WAV�t�@�C���̃p�X
 * @return          wave_file_writer�̃n���h��
 */
wave_file_writer* wave_file_writer_create(const char* path);

/*!
 * @brief           WAV�t�@�C�����쐬���A�w�肳�ꂽ�n���h���ŊJ���܂��B
 * @param *writer   wave_file_writer�̃n���h��
 * @param *path     WAV�t�@�C�����쐬����p�X
 */
void wave_file_writer_open(wave_file_writer* writer, const char* path);

/*!
 * @brief           �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C������܂��B
 * @param *writer   wave_file_writer�̃n���h��
 */
void wave_file_writer_close(const wave_file_writer* writer);

/*!
 * @brief                   �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C����PCM�̃t�H�[�}�b�g��ݒ肵�܂��B
 * @param *writer           wave_file_writer�̃n���h��
 * @param sample_rate       �T���v�����O���g��
 * @param bits_per_sample   �ʎq���r�b�g��
 * @param num_channels      �`�����l����
 */
void wave_file_writer_set_pcm_format(wave_file_writer* writer, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t num_channels);

/*!
 * @brief               �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C���̑��T���v������ݒ肵�܂��B
 * @param *writer       wave_file_writer�̃n���h��
 * @param num_samples   ���T���v����
 */
void wave_file_writer_set_num_samples(wave_file_writer* writer, uint32_t num_samples);

/*!
 * @brief           �w�肳�ꂽ�n���h���ō쐬����WAVE�t�@�C���ւ̃T���v���̏������݂��J�n���܂��B�ŏ��̃T���v���̏������ݑO�ɁA�K���A���̊֐����Ăяo���Ă��������B
 * @param *writer   wave_file_writer�̃n���h��
 */
void wave_file_writer_begin_write(const wave_file_writer* writer);

/*!
 * @brief           �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C���փT���v�����������݂܂��B�ŏ��ɂ��̊֐����Ăяo���O�ɁA�K���Awave_file_writer_begin_write �֐����Ăяo���Ă��������B
 * @param *writer   wave_file_writer�̃n���h��
 * @param sample    �������ރT���v��
 */
void wave_file_writer_write_sample(const wave_file_writer* writer, int32_t sample);

/*!
 * @brief           �w�肳�ꂽ�n���h���ō쐬����WAV�t�@�C���ւ̃T���v���̏������݂��I�����܂��B���ׂẴT���v�����������񂾌�ɁA�K�����̊֐����Ăяo���Ă��������B
 * @param *writer   wave_file_reader�̃n���h��
 */
void wave_file_writer_end_write(const wave_file_writer* writer);

#endif