#ifndef PATH_H
#define PATH_H
#define MAX_PATH 260

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STR "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STR "/"
#endif

#include <stdio.h>

/*!
 * @brief           �w�肳�ꂽ�p�X�̃t�@�C���̃o�C�g�����擾���܂��B
 * @param *path     �p�X
 * @return          �w�肳�ꂽ�p�X�̃t�@�C���̃o�C�g��
 */
fpos_t get_file_size(const char* file_name);

/*!
 * @brief           �w�肳�ꂽ�p�X����t�@�C�����������擾���܂��B
 * @param *path     �p�X
 * @return          �t�@�C����
 */
char* get_file_name(const char* path);

/*!
 * @brief           �w�肳�ꂽ�p�X����A�g���q�������t�@�C�����������擾���܂��B
 * @param *path     �p�X
 * @return          �g���q�������t�@�C����
 */
char* get_file_name_without_extension(const char* path);

/*!
 * @brief           �w�肳�ꂽ�p�X����g���q�������擾���܂��B
 * @param *path     �p�X
 * @return          �g���q
 */
char* get_extension(const char* path);

/*!
 * @brief           �w�肳�ꂽ�p�X����f�B���N�g�����������擾���܂��B
 * @param *path     �p�X
 * @return          �f�B���N�g��������
 */
char* get_directory_name(const char* path);

/*!
 * @brief               �w�肳�ꂽ�p�X�̃t�@�C���̊g���q��ύX�����p�X���擾���܂��B
 * @param path          �p�X
 * @param new_extension �V�����g���q
 */
void change_extension(char* path, const char* new_extension);

#endif