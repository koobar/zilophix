#ifndef FILE_ACCESS_HEADER_INCLUDED
#define FILE_ACCESS_HEADER_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������A�^�U�l��ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �^�U�l
 */
bool read_bool(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������16�r�b�g�����t��������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
int16_t read_int16(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������32�r�b�g�����t��������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
int32_t read_int32(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������ASCII������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
char read_char(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C�����當�����ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
const char* read_string(FILE* file, uint32_t bytes);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������8�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
uint8_t read_uint8(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������16�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
uint16_t read_uint16(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������32�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
uint32_t read_uint32(FILE* file);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C���ɐ^�U�l���������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_bool(FILE* file, bool value);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����ASCII�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_char(FILE* file, char value);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C���ɕ�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_string(FILE* file, const char* value, uint32_t bytes);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����8�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_uint8(FILE* file, uint8_t value);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����16�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_uint16(FILE* file, uint16_t value);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����32�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_uint32(FILE* file, uint32_t value);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����16�����t���r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_int16(FILE* file, int16_t value);

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����32�r�b�g�����t�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_int32(FILE* file, int32_t value);

#endif