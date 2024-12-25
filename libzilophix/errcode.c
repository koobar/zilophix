#include "./include/errcode.h"
#include <stdio.h>
#include <stdlib.h>

static bool on_error_exit = true;
static error_code last_error_code = ERROR_OK;

/*!
 * @brief           �w�肳�ꂽ�G���[�R�[�h�ŃG���[�����|�[�g���܂��B
 * @param error     �G���[�R�[�h
 */
void report_error(error_code error) {
    last_error_code = error;
    fprintf(stderr, "[Error] ERROR CODE = %#06x\n", error);

    if (on_error_exit) {    
        exit((int)error);
    }
}

/*!
 * @brief           �Ō�ɔ��������G���[�̃G���[�R�[�h���擾���܂��B
 * @return          �G���[�R�[�h
 */
error_code get_last_error_code() {
    return last_error_code;
}

/*!
 * @brief           �Ō�ɔ��������G���[�̃G���[�R�[�h���擾���܂��B
 * @param value     true�Ȃ�G���[�������ɑ����Ƀv���O�������I�����܂��Bfalse�Ȃ�A�v���O�������I�����܂���B
 */
void set_on_error_exit(bool value) {
    on_error_exit = value;
}
