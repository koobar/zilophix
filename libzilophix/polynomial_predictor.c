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
 * @brief               多項式予測器のハンドルを生成します。
 * @return              多項式予測器のハンドル
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
 * @brief               多項式予測器を解放します。
 * @param *predictor    多項式予測器のハンドル
 */
void polynomial_predictor_free(polynomial_predictor* predictor) {
    free(predictor->history);
}

/*!
 * @brief               多項式予測器を初期化します。
 * @param *predictor    多項式予測器のハンドル
 */
void polynomial_predictor_clear(polynomial_predictor* predictor) {
    memset(predictor->history, 0, sizeof(int32_t) * POLYNOMIAL_PREDICATOR_MAX_HISTORY);
}

/*!
 * @brief               指定されたハンドルの多項式予測器で、次に続くPCMサンプルを予測します。
 * @param *predictor    多項式予測器のハンドル
 * @return              予測されたPCMサンプル
 */
int32_t polynomial_predictor_predict(polynomial_predictor* predictor) {
    return PREDICT2(predictor->history[0], predictor->history[1], K);
}

/*!
 * @brief               多項式予測器を更新します。
 * @param *predictor    多項式予測器のハンドル
 * @param sample        PCMサンプル
 */
void polynomial_predictor_update(polynomial_predictor* predictor, int32_t sample) {
    memmove(&predictor->history[1], &predictor->history[0], (POLYNOMIAL_PREDICATOR_MAX_HISTORY - 1) * sizeof(int32_t));
    predictor->history[0] = sample;
}