#include "./include/path.h"
#include <stdlib.h>
#include <string.h>

/*!
 * @brief           Get file size.
 * @param *path     File path
 * @return          File size.
 */
fpos_t get_file_size(const char* file_name){
    fpos_t fsize = 0;
    FILE* fp;
    errno_t err;

    err = fopen_s(&fp, file_name, "rb");

    if (err != 0) {
        return 0;
    }

    /* Get file size. */
    fseek(fp, 0, SEEK_END);
    fgetpos(fp, &fsize);

    /* Close file. */
    fclose(fp);
    return fsize;
}

/*!
 * @brief           Get file name from full path.
 * @param *path     Full path.
 * @return          File name.
 */
char* get_file_name(const char* path) {
    char* file_name_ptr;
    char* result;
    size_t file_name_size;

    file_name_ptr = strrchr(path, PATH_SEPARATOR) + 1;
    file_name_size = strlen(file_name_ptr) + 1;

    result = (char*)malloc(sizeof(char) * file_name_size);
    snprintf(result, file_name_size, "%s", file_name_ptr);

    return result;
}

/*!
 * @brief           Get file name from full path without extension.
 * @param *path     Full Path
 * @return          File name without extension
 */
char* get_file_name_without_extension(const char* path) {
    char* name = get_file_name(path);
    size_t i;
    size_t len = strlen(name);

    for (i = len - 1; i != 0; --i) {
        if (name[i] == '.') {
            name[i] = '\0';
            break;
        }
    }

    return name;
}

/*!
 * @brief           Get extension from full path.
 * @param *path     Full path
 * @return          extension
 */
char* get_extension(const char* path) {
    const char* dot = NULL;
    char* result;
    size_t i, extension_size;
    size_t len = strlen(path);

    /* Get pointer of extension */
    for (i = len - 1; i != 0; --i) {
        if (path[i] == (char)'.') {
            dot = &path[i];
            break;
        }
    }

    if (dot == NULL) {
        return NULL;
    }

    /* Allocate memory */
    extension_size = strlen(dot);
    result = (char*)malloc(sizeof(char) * extension_size);

    if (result == NULL) {
        return NULL;
    }

    /* Copy characters to extension area */
    for (i = 0; i < extension_size; ++i) {
        result[i] = dot[i];
    }
    result[extension_size] = '\0';

    return result;
}

/*!
 * @brief           Get directory name from full path.
 * @param *path     Full path
 * @return          Directory name
 */
char* get_directory_name(const char* path){
    size_t size = strlen(path), directory_name_end_offset = 0, directory_name_size, i;
    char* result;

    for (i = size - 1; i != 0; --i) {
        if (path[i] == PATH_SEPARATOR) {
            directory_name_end_offset = i;
            break;
        }
    }

    /* Allocate memory */
    directory_name_size = directory_name_end_offset + 1;
    result = (char*)malloc(directory_name_size);

    if (result == NULL) {
        return NULL;
    }

    /* Copy characters to directory name area */
    for (i = 0; i < directory_name_size - 1; ++i) {
        result[i] = path[i];
    }
    result[directory_name_size - 1] = '\0';

    return result;
}

/*!
 * @brief               Change extension and full path.
 * @param path          Full path 
 * @param new_extension New extension
 */
void change_extension(char* path, const char* new_extension) {
    char* dot = NULL;
    size_t len = strlen(path);
    size_t i;

    for (i = len; i > 0; --i) {
        if (path[i - 1] == '.') {
            dot = &path[i - 1];
            break;
        }
    }

    if (dot != NULL) {
        strncpy(dot, new_extension, strlen(new_extension) + 1);
    }
    else {
        strncat(path, new_extension, strlen(new_extension));
    }
}