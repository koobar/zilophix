#ifndef POLYNOMIAL_PREDICTOR_HEADER_INCLUDED
#define POLYNOMIAL_PREDICTOR_HEADER_INCLUDED

#include <stdint.h>

typedef struct {
    int32_t* history;
} polynomial_predictor;

/*!
 * @brief               Create new instance of polynomial predictor.
 * @return              Pointer of created instance.
 */
polynomial_predictor* polynomial_predictor_create();

/*!
 * @brief               Release polynomial predictor.
 * @param *predictor    Pointer of polynomial predictor
 */
void polynomial_predictor_free(polynomial_predictor* predictor);

/*!
 * @brief               Initialize polynomial predictor.
 * @param *predictor    Pointer of polynomial predictor
 */
void polynomial_predictor_clear(polynomial_predictor* predictor);

/*!
 * @brief               Predict next sample.
 * @param *predictor    Pointer of polynomial predictor
 * @return              Prediction
 */
int32_t polynomial_predictor_predict(polynomial_predictor* predictor);

/*!
 * @brief               Update polynomial predictor.
 * @param *predictor    Pointer of polynomial predictor
 * @param sample        Actual sample.
 */
void polynomial_predictor_update(polynomial_predictor* predictor, int32_t sample);

#endif