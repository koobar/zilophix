#include "./include/sslms.h"
#include "./include/macro.h"
#include "./include/zilophix.h"
#include "./include/errcode.h"
#include <stdlib.h>
#include <string.h>

/* Shift factor table: { for 16-bits pcm, for 24-bits pcm }*/
static const int32_t shift_factor_table[2] = { 9, 8 };

/*!
 * @brief           Create new SSLMS filter instance.
 * @return          Pointer to created instance.
 */
sslms* sslms_create(uint8_t taps, uint8_t pcm_bits) {
    sslms* result = (sslms*)malloc(sizeof(sslms));

    if (result == NULL){
        report_error(ERROR_LMS_CANNOT_ALLOCATE_MEMORY);
        return NULL;
    }

    result->history = (int32_t*)calloc(taps, sizeof(int32_t));
    result->sign = (int8_t*)calloc(taps, sizeof(int8_t));
    result->weights = (int32_t*)calloc(taps, sizeof(int32_t));
    result->taps = taps;
    result->shift = shift_factor_table[RSHIFT(pcm_bits, 3) - 2];

    return result;
}

/*!
 * @brief           Release SSLMS filter.
 * @param *filter   Pointer to SSLMS filter.
 */
void sslms_free(sslms* filter) {
    free(filter->history);
    free(filter->sign);
    free(filter->weights);
}

/*!
 * @brief           Initialize SSLMS filter.
 * @param *filter   Pointer to SSLMS filter.
 */

void sslms_clear(sslms* filter) {
    memset(filter->history, 0, sizeof(int32_t) * filter->taps);
    memset(filter->sign, 0, sizeof(int8_t) * filter->taps);
    memset(filter->weights, 0, sizeof(int32_t) * filter->taps);
}

/*!
 * @brief           Predict next sample.
 * @param *filter   Pointer to SSLMS filter.
 * @return          Prediction
 */
int32_t sslms_predict(sslms* filter) {
    int32_t sum = 0;
    uint32_t i;

    for (i = 0; i < filter->taps; ++i) {
        sum += RSHIFT(filter->weights[i] * filter->history[i], filter->shift);
    }

    return sum;
}

/*!
 * @brief           Update SSLMS filter.
 * @param *filter   Pointer to SSLMS filter.
 * @param sample    Actual sample.
 * @param residual  Prediction residual.
 */
void sslms_update(sslms* filter, int32_t sample, int32_t residual) {
    int8_t sgn = SIGN(residual);
    uint32_t i;

    /* Update weights. */
    for (i = 0; i < filter->taps; ++i) {
        filter->weights[i] += sgn * filter->sign[i];
    }

    /* Move the past samples and past codes, and store the latest samples and codes. */
    memmove(&filter->history[1], &filter->history[0], (filter->taps - 1) * sizeof(int32_t));
    memmove(&filter->sign[1], &filter->sign[0], (filter->taps - 1) * sizeof(int8_t));
    filter->history[0] = sample;
    filter->sign[0] = SIGN(sample);
}