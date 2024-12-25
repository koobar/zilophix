#include "./include/file_access.h"
#include "./include/errcode.h"
#include <stdlib.h>
#include <string.h>

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������A�^�U�l��ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �^�U�l
 */
bool read_bool(FILE* file) {
    uint8_t value;
    size_t actual_read = fread(&value, sizeof(uint8_t), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_BOOL);
    }

    return value == 1;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������16�r�b�g�����t��������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
int16_t read_int16(FILE* file) {
    int16_t value;
    size_t actual_read = fread(&value, sizeof(int16_t), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_INT16);
    }

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������32�r�b�g�����t��������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
int32_t read_int32(FILE* file) {
    int32_t value;
    size_t actual_read = fread(&value, sizeof(int32_t), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_INT32);
    }

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������ASCII������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
char read_char(FILE* file) {
    char value;
    size_t actual_read = fread(&value, sizeof(char), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_CHAR);
    }

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������ASCII������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
const char* read_string(FILE* file, uint32_t bytes) {
    uint32_t i;
    char* result = (char*)malloc(bytes);

    if (result == NULL) {
        return NULL;
    }

    for (i = 0; i < bytes; ++i) {
        result[i] = read_char(file);
    }

    return result;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������8�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
uint8_t read_uint8(FILE* file) {
    uint8_t value;
    size_t actual_read = fread(&value, sizeof(uint8_t), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_UINT8);
    }

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������16�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
uint16_t read_uint16(FILE* file) {
    uint16_t value;
    size_t actual_read = fread(&value, sizeof(uint16_t), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_UINT16);
    }

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C������32�r�b�g������ǂݍ��݂܂��B
 * @param file  �t�@�C���|�C���^
 * @return      �ǂݍ��܂ꂽ�l
 */
uint32_t read_uint32(FILE* file) {
    uint32_t value;
    size_t actual_read = fread(&value, sizeof(uint32_t), 1, file);

    if (actual_read < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_READ_UINT32);
    }

    return value;
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C���ɐ^�U�l���������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_bool(FILE* file, bool value) {
    size_t actual_write = 0;
    const uint8_t _true = 1;
    const uint8_t _false = 0;

    if (value) {
        actual_write = fwrite(&_true, sizeof(uint8_t), 1, file);
    }
    else {
        actual_write = fwrite(&_false, sizeof(uint8_t), 1, file);
    }

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_BOOL);
    }
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����ASCII�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_char(FILE* file, char value) {
    size_t actual_write = fwrite(&value, sizeof(char), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_CHAR);
    }
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C���ɕ�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_string(FILE* file, const char* value, uint32_t bytes) {
    uint8_t i;

    for (i = 0; i < bytes - 1; ++i) {
        write_char(file, value[i]);
    }
    write_char(file, '\0');         /* ���[���� */
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����8�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_uint8(FILE* file, uint8_t value) {
    size_t actual_write = fwrite(&value, sizeof(uint8_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT8);
    }
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����16�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_uint16(FILE* file, uint16_t value) {
    size_t actual_write = fwrite(&value, sizeof(uint16_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT16);
    }
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����32�r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_uint32(FILE* file, uint32_t value) {
    size_t actual_write = fwrite(&value, sizeof(uint32_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT32);
    }
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����16�����t���r�b�g�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_int16(FILE* file, int16_t value) {
    size_t actual_write = fwrite(&value, sizeof(int16_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT16);
    }
}

/*!
 * @brief       �w�肳�ꂽ�t�@�C���|�C���^�������t�@�C����32�r�b�g�����t�������������ށB
 * @param file  �t�@�C���|�C���^
 * @param value �������ޒl
 */
void write_int32(FILE* file, int32_t value) {
    size_t actual_write = fwrite(&value, sizeof(int32_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT32);
    }
}