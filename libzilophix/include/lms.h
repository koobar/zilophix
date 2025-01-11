#ifndef LMS_HEADER_INCLUDED
#define LMS_HEADER_INCLUDED

#define LMS_MAX_TAPS    32

#include <stdint.h>

typedef struct {
    uint32_t taps;              /* Taps */
    uint8_t shift;              /* Shift coefficients */
    int32_t* history;           /* The pointer of past samples.*/
    int8_t* sign;               /* The pointer of past sign. */
    int32_t* weights;           /* The pointer of weights. */
} lms;

/*!
 * @brief           Create new SSLMS filter instance.
 * @return          Pointer of created instance.
 */
lms* lms_create(uint8_t taps, uint8_t pcm_bits);

/*!
 * @brief           Release SSLMS filter.
 * @param *filter   Pointer of SSLMS filter.
 */
void lms_free(lms* filter);

/*!
 * @brief           Initialize SSLMS filter.
 * @param *filter   Pointer of SSLMS filter.
 */
void lms_clear(lms* filter);

/*!
 * @brief           Predict next sample.
 * @param *filter   Pointer of SSLMS filter.
 * @return          Prediction
 */
int32_t lms_predict(lms* filter);

/*!
 * @brief           Update SSLMS filter.
 * @param *filter   Pointer of SSLMS filter.
 * @param sample    Actual sample.
 * @param residual  Prediction residual.
 */
void lms_update(lms* filter, int32_t sample, int32_t residual);

#endif