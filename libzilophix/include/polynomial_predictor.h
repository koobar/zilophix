#ifndef POLYNOMIAL_PREDICTOR_HEADER_INCLUDED
#define POLYNOMIAL_PREDICTOR_HEADER_INCLUDED

#include <stdint.h>

typedef struct {
    int32_t* history;
} polynomial_predictor;

/*!
 * @brief               Create new polynomial_predictor.
 * @return              Pointer to created polynomial_predictor.
 */
polynomial_predictor* polynomial_predictor_create();

/*!
 * @brief               Release polynomial predictor.
 * @param *predictor    Pointer to polynomial_predictor
 */
void polynomial_predictor_free(polynomial_predictor* predictor);

/*!
 * @brief               Initialize polynomial_predictor.
 * @param *predictor    Pointer to polynomial_predictor
 */
void polynomial_predictor_clear(polynomial_predictor* predictor);

/*!
 * @brief               Predict next sample.
 * @param *predictor    Pointer to polynomial_predictor
 * @return              Prediction
 */
int32_t polynomial_predictor_predict(polynomial_predictor* predictor);

/*!
 * @brief               Update polynomial_predictor.
 * @param *predictor    Pointer to polynomial_predictor
 * @param sample        Actual sample.
 */
void polynomial_predictor_update(polynomial_predictor* predictor, int32_t sample);

#endif