#ifndef LMS_HEADER_INCLUDED
#define LMS_HEADER_INCLUDED

#define LMS_MAX_TAPS    32

#include <stdint.h>

typedef struct {
    uint32_t taps;              /* �\������ */
    uint8_t shift;              /* �V�t�g�W�� */
    int32_t* history;           /* �ߋ��T���v���̈�̃|�C���^ */
    int8_t* sign;               /* �ߋ��T���v���̕����̈�̃|�C���^ */
    int32_t* weights;           /* �d�݌W���̈�̃|�C���^ */
} lms;

/*!
 * @brief           LMS�t�B���^�̃n���h���𐶐����܂��B
 * @return          LMS�t�B���^�̃n���h��
 */
lms* lms_create(uint8_t taps, uint8_t pcm_bits);

/*!
 * @brief           LMS�t�B���^��������܂��B
 * @param *filter   LMS�t�B���^�̃n���h��
 */
void lms_free(lms* filter);

/*!
 * @brief           LMS�t�B���^�����������܂��B
 * @param *filter   LMS�t�B���^�̃n���h��
 */
void lms_clear(lms* filter);

/*!
 * @brief           PCM�T���v����\�����܂��B
 * @param *filter   LMS�t�B���^�̃n���h��
 * @return          �\�����ꂽPCM�T���v��
 */
int32_t lms_predict(lms* filter);

/*!
 * @brief           LMS�t�B���^���X�V���܂��B
 * @param *filter   LMS�t�B���^�̃n���h��
 * @param sample    ���ۂ�PCM�T���v��
 * @param residual  ���ۂ�PCM�T���v���ƁA�\�����ꂽPCM�T���v���̍���
 */
void lms_update(lms* filter, int32_t sample, int32_t residual);

#endif