#include "./include/macro.h"
#include "./include/errcode.h"
#include "./include/polynomial_predictor.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PREDICT(p, k)           (RSHIFT(p * (LSHIFT(1, k) - 1), k))
#define PREDICT2(p1, p2, k)     (LSHIFT(PREDICT((p1), (k)), 1) - (PREDICT((p2), (k - 1))))
#define K 4

/*!
 * @brief               �������\����̃n���h���𐶐����܂��B
 * @return              �������\����̃n���h��
 */
polynomial_predictor* polynomial_predictor_create() {
    polynomial_predictor* result = (polynomial_predictor*)malloc(sizeof(polynomial_predictor));

    if (result == NULL) {
        report_error(ERROR_POLYNOMIAL_PREDICTOR_CANNOT_ALLOCATE_MEMORY);
        return NULL;
    }

    result->history = (int32_t*)calloc(POLYNOMIAL_PREDICATOR_MAX_HISTORY, sizeof(int32_t));
    return result;
}

/*!
 * @brief               �������\�����������܂��B
 * @param *predictor    �������\����̃n���h��
 */
void polynomial_predictor_free(polynomial_predictor* predictor) {
    free(predictor->history);
}

/*!
 * @brief               �������\��������������܂��B
 * @param *predictor    �������\����̃n���h��
 */
void polynomial_predictor_clear(polynomial_predictor* predictor) {
    memset(predictor->history, 0, sizeof(int32_t) * POLYNOMIAL_PREDICATOR_MAX_HISTORY);
}

/*!
 * @brief               �w�肳�ꂽ�n���h���̑������\����ŁA���ɑ���PCM�T���v����\�����܂��B
 * @param *predictor    �������\����̃n���h��
 * @return              �\�����ꂽPCM�T���v��
 */
int32_t polynomial_predictor_predict(polynomial_predictor* predictor) {
    return PREDICT2(predictor->history[0], predictor->history[1], K);
}

/*!
 * @brief               �������\������X�V���܂��B
 * @param *predictor    �������\����̃n���h��
 * @param sample        PCM�T���v��
 */
void polynomial_predictor_update(polynomial_predictor* predictor, int32_t sample) {
    memmove(&predictor->history[1], &predictor->history[0], (POLYNOMIAL_PREDICATOR_MAX_HISTORY - 1) * sizeof(int32_t));
    predictor->history[0] = sample;
}