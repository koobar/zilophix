#ifndef WAVE_FILE_READER_HEADER_INCLUDED
#define WAVE_FILE_READER_HEADER_INCLUDED

#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* wave_file;
    uint32_t stream_size;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
    uint32_t num_samples;
    uint32_t num_samples_read;
    uint32_t avr_bytes_per_sec;
    uint16_t block_size;
} wave_file_reader;

/*!
 * @brief           wave_file_reader�̃n���h���𐶐����܂��B
 * @param path      �ǂݍ���WAV�t�@�C���̃p�X
 * @return          wave_file_reader�̃n���h��
 */
wave_file_reader* wave_file_reader_create(const char* path);

/*!
 * @brief           WAV�t�@�C�����J���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @param *path     �t�@�C���p�X
 */
void wave_file_reader_open(wave_file_reader* reader, const char* path);

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C������܂��B
 * @param *reader   wave_file_reader�̃n���h��
 */
void wave_file_reader_close(const wave_file_reader* reader);

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̃T���v�����O���g�����擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �T���v�����O���g��
 */
uint32_t wave_file_reader_get_sample_rate(const wave_file_reader* reader);

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̗ʎq���r�b�g�����擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �ʎq���r�b�g��
 */
uint16_t wave_file_reader_get_bits_per_sample(const wave_file_reader* reader);

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̃`�����l�������擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �`�����l����
 */
uint16_t wave_file_reader_get_num_channels(const wave_file_reader* reader);

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C���̑��T���v�������擾���܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          ���T���v����
 */
uint32_t wave_file_reader_get_num_samples(const wave_file_reader* reader);

/*!
 * @brief           �w�肳�ꂽ�n���h���ŊJ���ꂽWAV�t�@�C�����玟�̃T���v����ǂݍ��݂܂��B
 * @param *reader   wave_file_reader�̃n���h��
 * @return          �T���v��
 */
int32_t wave_file_reader_read_sample(wave_file_reader* reader);

#endif