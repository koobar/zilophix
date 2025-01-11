#include "./include/file_access.h"
#include "./include/errcode.h"
#include <stdlib.h>
#include <string.h>

/*!
 * @brief       Reads a boolean value from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The boolean value read
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
 * @brief       Reads a 16-bit signed integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
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
 * @brief       Reads a 32-bit signed integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
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
 * @brief       Reads an ASCII character from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The character read
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
 * @brief       Reads an ASCII character from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The character read
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
 * @brief       Reads an 8-bit unsigned integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
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
 * @brief       Reads a 16-bit unsigned integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
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
 * @brief       Reads a 32-bit unsigned integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
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
 * @brief       Writes a boolean value to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
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
 * @brief       Writes an ASCII character to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_char(FILE* file, char value) {
    size_t actual_write = fwrite(&value, sizeof(char), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_CHAR);
    }
}

/*!
 * @brief       Writes a string to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_string(FILE* file, const char* value, uint32_t bytes) {
    uint8_t i;

    for (i = 0; i < bytes - 1; ++i) {
        write_char(file, value[i]);
    }
    write_char(file, '\0');         /* Null terminator */
}

/*!
 * @brief       Writes an 8-bit unsigned integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_uint8(FILE* file, uint8_t value) {
    size_t actual_write = fwrite(&value, sizeof(uint8_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT8);
    }
}

/*!
 * @brief       Writes a 16-bit unsigned integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_uint16(FILE* file, uint16_t value) {
    size_t actual_write = fwrite(&value, sizeof(uint16_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT16);
    }
}

/*!
 * @brief       Writes a 32-bit unsigned integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_uint32(FILE* file, uint32_t value) {
    size_t actual_write = fwrite(&value, sizeof(uint32_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT32);
    }
}

/*!
 * @brief       Writes a 16-bit signed integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_int16(FILE* file, int16_t value) {
    size_t actual_write = fwrite(&value, sizeof(int16_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT16);
    }
}

/*!
 * @brief       Writes a 32-bit signed integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_int32(FILE* file, int32_t value) {
    size_t actual_write = fwrite(&value, sizeof(int32_t), 1, file);

    if (actual_write < 1) {
        report_error(ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT32);
    }
}