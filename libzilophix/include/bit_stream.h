#ifndef BIT_STREAM_HEADER_INCLUDED
#define BIT_STREAM_HEADER_INCLUDED

#define BIT_STREAM_MODE_READ	0x00
#define BIT_STREAM_MODE_WRITE	0x01

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    FILE* file;
    uint32_t mode;
    uint32_t buffer;
    uint32_t buffer_position;
    uint32_t max_buffer_bytes;
    uint32_t max_buffer_bits;
} bit_stream;

/*!
 * @brief           �r�b�g�X�g���[���̗̈���m�ۂ��A���̃n���h����Ԃ��܂��B
 * @param *file     �r�b�g�X�g���[���ň����t�@�C���̃t�@�C���n���h��
 * @param mode      �r�b�g�X�g���[���̃��[�h
 * @return          �r�b�g�X�g���[���̃n���h��
 */
bit_stream* bit_stream_create(FILE* file, uint32_t mode);

/*!
 * @brief           �r�b�g�X�g���[�������������܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 */
void bit_stream_init(bit_stream* stream);

/*!
 * @brief           �r�b�g�X�g���[������1�r�b�g�ǂݍ��݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @return          �ǂݍ��܂ꂽ�r�b�g
 */
bool bit_stream_read_bit(bit_stream* stream);

/*!
 * @brief           �r�b�g�X�g���[������C�Ӄr�b�g���̐�����ǂݍ��݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @param bits      �ǂݍ��ސ����̃r�b�g��
 * @return          �ǂݍ��܂ꂽ����
 */
uint32_t bit_stream_read_uint(bit_stream* stream, uint32_t bits);

/*!
 * @brief           �r�b�g�X�g���[������unary���������ꂽ������ǂݍ��݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 */
uint32_t bit_stream_read_unary(bit_stream* stream);

/*!
 * @brief           �r�b�g�X�g���[������Rice���������ꂽ�����t��������ǂݍ��݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @param bits      �ǂݍ��ސ����̃r�b�g��
 * @return          �ǂݍ��܂ꂽ����
 */
uint32_t bit_stream_read_rice_signed(bit_stream* stream, uint32_t parameter);

/*!
 * @brief           �r�b�g�X�g���[����1�r�b�g�������݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @param bit       �������ރr�b�g
 */
void bit_stream_write_bit(bit_stream* stream, bool bit);

/*!
 * @brief           �r�b�g�X�g���[���ɔC�Ӄr�b�g���̐������������݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @param value     �������ސ���
 * @param num_bits  �������ސ����̃r�b�g��
 */
void bit_stream_write_uint(bit_stream* stream, uint32_t value, uint32_t num_bits);

/*!
 * @brief           �r�b�g�X�g���[���ɐ�����unary�����Ƃ��ď������݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @param value     �������ސ���
 */
void bit_stream_write_unary(bit_stream* stream, uint32_t value);

/*!
 * @brief           �r�b�g�X�g���[���ɐ�����Rice�����Ƃ��ď������݂܂��B
 * @param *stream   �r�b�g�X�g���[���̃n���h��
 * @param value     �������ސ���
 */
void bit_stream_write_signed_rice(bit_stream* stream, int32_t value, uint32_t parameter);

/*!
 * @brief			�o�b�t�@�Ɏc���Ă���A�t�@�C���|�C���^�������t�@�C���ɏ������܂�Ă��Ȃ��l�𖳏����ɏ������݂܂��B
 * @param *stream	�r�b�g�X�g���[���̃n���h��
 */
void bit_stream_close(bit_stream* stream);

#endif