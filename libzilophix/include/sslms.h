#ifndef SSLMS_HEADER_INCLUDED
#define SSLMS_HEADER_INCLUDED

#define SSLMS_MAX_TAPS    32

#include <stdint.h>

typedef struct {
    uint32_t taps;              /* Taps */
    uint8_t shift;              /* Shift coefficients */
    int32_t* history;           /* The pointer to past samples.*/
    int8_t* sign;               /* The pointer to past sign. */
    int32_t* weights;           /* The pointer to weights. */
} sslms;

/*!
 * @brief           Create new SSLMS filter instance.
 * @return          Pointer to created instance.
 */
sslms* sslms_create(uint8_t taps, uint8_t shift);

/*!
 * @brief           Release SSLMS filter.
 * @param *filter   Pointer to SSLMS filter.
 */
void sslms_free(sslms* filter);

/*!
 * @brief           Initialize SSLMS filter.
 * @param *filter   Pointer to SSLMS filter.
 */
void sslms_clear(sslms* filter);

/*!
 * @brief           Predict next sample.
 * @param *filter   Pointer to SSLMS filter.
 * @return          Prediction
 */
int32_t sslms_predict(sslms* filter);

/*!
 * @brief           Update SSLMS filter.
 * @param *filter   Pointer to SSLMS filter.
 * @param sample    Actual sample.
 * @param residual  Prediction residual.
 */
void sslms_update(sslms* filter, int32_t sample, int32_t residual);

#endif