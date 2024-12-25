#ifndef POLYNOMIAL_PREDICTOR_HEADER_INCLUDED
#define POLYNOMIAL_PREDICTOR_HEADER_INCLUDED

#include <stdint.h>

typedef struct {
    int32_t* history;
} polynomial_predictor;

/*!
 * @brief               �������\����̃n���h���𐶐����܂��B
 * @return              �������\����̃n���h��
 */
polynomial_predictor* polynomial_predictor_create();

/*!
 * @brief               �������\�����������܂��B
 * @param *predictor    �������\����̃n���h��
 */
void polynomial_predictor_free(polynomial_predictor* predictor);

/*!
 * @brief               �������\��������������܂��B
 * @param *predictor    �������\����̃n���h��
 */
void polynomial_predictor_clear(polynomial_predictor* predictor);

/*!
 * @brief               �w�肳�ꂽ�n���h���̑������\����ŁA���ɑ���PCM�T���v����\�����܂��B
 * @param *predictor    �������\����̃n���h��
 * @return              �\�����ꂽPCM�T���v��
 */
int32_t polynomial_predictor_predict(polynomial_predictor* predictor);

/*!
 * @brief               �������\������X�V���܂��B
 * @param *predictor    �������\����̃n���h��
 * @param sample        PCM�T���v��
 */
void polynomial_predictor_update(polynomial_predictor* predictor, int32_t sample);

#endif