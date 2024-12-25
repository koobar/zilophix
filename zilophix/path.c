#include "./include/path.h"
#include <stdlib.h>
#include <string.h>

/*!
 * @brief           �w�肳�ꂽ�p�X�̃t�@�C���̃o�C�g�����擾���܂��B
 * @param *path     �p�X
 * @return          �w�肳�ꂽ�p�X�̃t�@�C���̃o�C�g��
 */
fpos_t get_file_size(const char* file_name){
    fpos_t fsize = 0;
    FILE* fp;
    errno_t err;

    err = fopen_s(&fp, file_name, "rb");

    if (err != 0) {
        return 0;
    }

    /* �t�@�C���T�C�Y���擾 */
    fseek(fp, 0, SEEK_END);
    fgetpos(fp, &fsize);

    /* �t�@�C���T�C�Y���擾���邽�߂ɊJ���������Ȃ̂ŁA�����ŕ��Ă��܂��Ă��悢 */
    fclose(fp);
    return fsize;
}

/*!
 * @brief           �w�肳�ꂽ�p�X����t�@�C�����������擾���܂��B
 * @param *path     �p�X
 * @return          �t�@�C����
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
 * @brief           �w�肳�ꂽ�p�X����A�g���q�������t�@�C�����������擾���܂��B
 * @param *path     �p�X
 * @return          �g���q�������t�@�C����
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
 * @brief           �w�肳�ꂽ�p�X����g���q�������擾���܂��B
 * @param *path     �p�X
 * @return          �g���q
 */
char* get_extension(const char* path) {
    const char* dot = NULL;
    char* result;
    size_t i, extension_size;
    size_t len = strlen(path);

    /* �����ɗ^����ꂽ�p�X�̂����A�g���q�̊J�n�ʒu�̃|�C���^���擾���� */
    for (i = len - 1; i != 0; --i) {
        if (path[i] == (char)'.') {
            dot = &path[i];
            break;
        }
    }

    /* �|�C���^�̎擾���s�i�g���q���݂���Ȃ��j�Ȃ�NULL��Ԃ� */
    if (dot == NULL) {
        return NULL;
    }

    /* �g���q�̗̈���m�� */
    extension_size = strlen(dot);
    result = (char*)malloc(sizeof(char) * extension_size);

    /* �g���q�̈�̊m�ۂɎ��s�����ꍇ��NULL��Ԃ� */
    if (result == NULL) {
        return NULL;
    }

    /* �g���q�������g���q�̗̈�ɃR�s�[ */
    for (i = 0; i < extension_size; ++i) {
        result[i] = dot[i];
    }
    result[extension_size] = '\0';

    return result;
}

/*!
 * @brief           �w�肳�ꂽ�p�X����f�B���N�g�����������擾���܂��B
 * @param *path     �p�X
 * @return          �f�B���N�g��������
 */
char* get_directory_name(const char* path){
    size_t size = strlen(path), directory_name_end_offset = 0, directory_name_size, i;
    char* result;

    /* �f�B���N�g���������̏I���ʒu���擾 */
    for (i = size - 1; i != 0; --i) {
        if (path[i] == PATH_SEPARATOR) {
            directory_name_end_offset = i;
            break;
        }
    }

    /* �f�B���N�g�����������i�[����̈���m�� */
    directory_name_size = directory_name_end_offset + 1;
    result = (char*)malloc(directory_name_size);

    /* �m�ۂł��Ă��Ȃ����NULL��Ԃ� */
    if (result == NULL) {
        return NULL;
    }

    /* �f�B���N�g�����������f�B���N�g�����̈�ɃR�s�[ */
    for (i = 0; i < directory_name_size - 1; ++i) {
        result[i] = path[i];
    }
    result[directory_name_size - 1] = '\0';

    return result;
}

/*!
 * @brief               �w�肳�ꂽ�p�X�̃t�@�C���̊g���q��ύX�����p�X���擾���܂��B
 * @param path          �p�X
 * @param new_extension �V�����g���q
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
        // �����̊g���q��u��������
        strncpy(dot, new_extension, strlen(new_extension) + 1);
    }
    else {
        // �V�����g���q��ǉ�
        strncat(path, new_extension, strlen(new_extension));
    }
}