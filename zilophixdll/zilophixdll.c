#include "./include/zilophixdll.h"

/*!
 * @brief       �Ō�ɔ��������G���[�̃G���[�R�[�h���擾���܂��B
 * @return      �Ō�ɔ��������G���[�̃G���[�R�[�h
 */
error_code GetLastErrorCode() {
    return get_last_error_code();
}