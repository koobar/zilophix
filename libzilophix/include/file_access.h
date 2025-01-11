#ifndef FILE_ACCESS_HEADER_INCLUDED
#define FILE_ACCESS_HEADER_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*!
 * @brief       Reads a boolean value from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The boolean value read
 */
bool read_bool(FILE* file);

/*!
 * @brief       Reads a 16-bit signed integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
 */
int16_t read_int16(FILE* file);

/*!
 * @brief       Reads a 32-bit signed integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
 */
int32_t read_int32(FILE* file);

/*!
 * @brief       Reads an ASCII character from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The character read
 */
char read_char(FILE* file);

/*!
 * @brief       Reads a string from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The string read
 */
const char* read_string(FILE* file, uint32_t bytes);

/*!
 * @brief       Reads an 8-bit unsigned integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
 */
uint8_t read_uint8(FILE* file);

/*!
 * @brief       Reads a 16-bit unsigned integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
 */
uint16_t read_uint16(FILE* file);

/*!
 * @brief       Reads a 32-bit unsigned integer from the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @return      The value read
 */
uint32_t read_uint32(FILE* file);

/*!
 * @brief       Writes a boolean value to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_bool(FILE* file, bool value);

/*!
 * @brief       Writes an ASCII character to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_char(FILE* file, char value);

/*!
 * @brief       Writes a string to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_string(FILE* file, const char* value, uint32_t bytes);

/*!
 * @brief       Writes an 8-bit unsigned integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_uint8(FILE* file, uint8_t value);

/*!
 * @brief       Writes a 16-bit unsigned integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_uint16(FILE* file, uint16_t value);

/*!
 * @brief       Writes a 32-bit unsigned integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_uint32(FILE* file, uint32_t value);

/*!
 * @brief       Writes a 16-bit signed integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_int16(FILE* file, int16_t value);

/*!
 * @brief       Writes a 32-bit signed integer to the file pointed to by the given file pointer.
 * @param file  The file pointer
 * @param value The value to be written
 */
void write_int32(FILE* file, int32_t value);


#endif