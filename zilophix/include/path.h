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
 * @brief           Get file size.
 * @param *path     File path
 * @return          File size.
 */
fpos_t get_file_size(const char* file_name);

/*!
 * @brief           Get file name from full path.
 * @param *path     Full path.
 * @return          File name.
 */
char* get_file_name(const char* path);

/*!
 * @brief           Get file name from full path without extension.
 * @param *path     Full Path
 * @return          File name without extension
 */
char* get_file_name_without_extension(const char* path);

/*!
 * @brief           Get extension from full path.
 * @param *path     Full path
 * @return          extension
 */
char* get_extension(const char* path);

/*!
 * @brief           Get directory name from full path.
 * @param *path     Full path
 * @return          Directory name
 */
char* get_directory_name(const char* path);

/*!
 * @brief               Change extension and full path.
 * @param path          Full path
 * @param new_extension New extension
 */
void change_extension(char* path, const char* new_extension);

#endif