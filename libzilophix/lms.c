#include "./include/lms.h"
#include "./include/macro.h"
#include "./include/zilophix.h"
#include "./include/errcode.h"
#include <stdlib.h>
#include <string.h>

/* Shift factor table: { for 16-bits pcm, for 24-bits pcm }*/
static const int32_t shift_factor_table[2] = { 9, 8 };

/*!
 * @brief           Create new SSLMS filter instance.
 * @return          Pointer of created instance.
 */
lms* lms_create(uint8_t taps, uint8_t pcm_bits) {
    lms* result = (lms*)malloc(sizeof(lms));

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
 * @param *filter   Pointer of SSLMS filter.
 */
void lms_free(lms* filter) {
    free(filter->history);
    free(filter->sign);
    free(filter->weights);
}

/*!
 * @brief           Initialize SSLMS filter.
 * @param *filter   Pointer of SSLMS filter.
 */

void lms_clear(lms* filter) {
    memset(filter->history, 0, sizeof(int32_t) * filter->taps);
    memset(filter->sign, 0, sizeof(int8_t) * filter->taps);
    memset(filter->weights, 0, sizeof(int32_t) * filter->taps);
}

/*!
 * @brief           Predict next sample.
 * @param *filter   Pointer of SSLMS filter.
 * @return          Prediction
 */
int32_t lms_predict(lms* filter) {
    int32_t sum = 0;

    /* It's a stupid way of writing it, but it works faster than a for statement. */
    switch (filter->taps){
        case 32: sum += RSHIFT(filter->weights[31] * filter->history[31], filter->shift);
        case 31: sum += RSHIFT(filter->weights[30] * filter->history[30], filter->shift);
        case 30: sum += RSHIFT(filter->weights[29] * filter->history[29], filter->shift);
        case 29: sum += RSHIFT(filter->weights[28] * filter->history[28], filter->shift);
        case 28: sum += RSHIFT(filter->weights[27] * filter->history[27], filter->shift);
        case 27: sum += RSHIFT(filter->weights[26] * filter->history[26], filter->shift);
        case 26: sum += RSHIFT(filter->weights[25] * filter->history[25], filter->shift);
        case 25: sum += RSHIFT(filter->weights[24] * filter->history[24], filter->shift);
        case 24: sum += RSHIFT(filter->weights[23] * filter->history[23], filter->shift);
        case 23: sum += RSHIFT(filter->weights[22] * filter->history[22], filter->shift);
        case 22: sum += RSHIFT(filter->weights[21] * filter->history[21], filter->shift);
        case 21: sum += RSHIFT(filter->weights[20] * filter->history[20], filter->shift);
        case 20: sum += RSHIFT(filter->weights[19] * filter->history[19], filter->shift);
        case 19: sum += RSHIFT(filter->weights[18] * filter->history[18], filter->shift);
        case 18: sum += RSHIFT(filter->weights[17] * filter->history[17], filter->shift);
        case 17: sum += RSHIFT(filter->weights[16] * filter->history[16], filter->shift);
        case 16: sum += RSHIFT(filter->weights[15] * filter->history[15], filter->shift);
        case 15: sum += RSHIFT(filter->weights[14] * filter->history[14], filter->shift);
        case 14: sum += RSHIFT(filter->weights[13] * filter->history[13], filter->shift);
        case 13: sum += RSHIFT(filter->weights[12] * filter->history[12], filter->shift);
        case 12: sum += RSHIFT(filter->weights[11] * filter->history[11], filter->shift);
        case 11: sum += RSHIFT(filter->weights[10] * filter->history[10], filter->shift);
        case 10: sum += RSHIFT(filter->weights[9] * filter->history[9], filter->shift);
        case 9: sum += RSHIFT(filter->weights[8] * filter->history[8], filter->shift);
        case 8: sum += RSHIFT(filter->weights[7] * filter->history[7], filter->shift);
        case 7: sum += RSHIFT(filter->weights[6] * filter->history[6], filter->shift);
        case 6: sum += RSHIFT(filter->weights[5] * filter->history[5], filter->shift);
        case 5: sum += RSHIFT(filter->weights[4] * filter->history[4], filter->shift);
        case 4: sum += RSHIFT(filter->weights[3] * filter->history[3], filter->shift);
        case 3: sum += RSHIFT(filter->weights[2] * filter->history[2], filter->shift);
        case 2: sum += RSHIFT(filter->weights[1] * filter->history[1], filter->shift);
        case 1: sum += RSHIFT(filter->weights[0] * filter->history[0], filter->shift);
    }

    return sum;
}

/*!
 * @brief           Update SSLMS filter.
 * @param *filter   Pointer of SSLMS filter.
 * @param sample    Actual sample.
 * @param residual  Prediction residual.
 */
void lms_update(lms* filter, int32_t sample, int32_t residual) {
    int8_t sgn = SIGN(residual);

    /* It's a stupid way of writing it, but it works faster than a for statement. */
    switch (filter->taps){
        case 32: filter->weights[31] += sgn * filter->sign[31];
        case 31: filter->weights[30] += sgn * filter->sign[30];
        case 30: filter->weights[29] += sgn * filter->sign[29];
        case 29: filter->weights[28] += sgn * filter->sign[28];
        case 28: filter->weights[27] += sgn * filter->sign[27];
        case 27: filter->weights[26] += sgn * filter->sign[26];
        case 26: filter->weights[25] += sgn * filter->sign[25];
        case 25: filter->weights[24] += sgn * filter->sign[24];
        case 24: filter->weights[23] += sgn * filter->sign[23];
        case 23: filter->weights[22] += sgn * filter->sign[22];
        case 22: filter->weights[21] += sgn * filter->sign[21];
        case 21: filter->weights[20] += sgn * filter->sign[20];
        case 20: filter->weights[19] += sgn * filter->sign[19];
        case 19: filter->weights[18] += sgn * filter->sign[18];
        case 18: filter->weights[17] += sgn * filter->sign[17];
        case 17: filter->weights[16] += sgn * filter->sign[16];
        case 16: filter->weights[15] += sgn * filter->sign[15];
        case 15: filter->weights[14] += sgn * filter->sign[14];
        case 14: filter->weights[13] += sgn * filter->sign[13];
        case 13: filter->weights[12] += sgn * filter->sign[12];
        case 12: filter->weights[11] += sgn * filter->sign[11];
        case 11: filter->weights[10] += sgn * filter->sign[10];
        case 10: filter->weights[9] += sgn * filter->sign[9];
        case 9: filter->weights[8] += sgn * filter->sign[8];
        case 8: filter->weights[7] += sgn * filter->sign[7];
        case 7: filter->weights[6] += sgn * filter->sign[6];
        case 6: filter->weights[5] += sgn * filter->sign[5];
        case 5: filter->weights[4] += sgn * filter->sign[4];
        case 4: filter->weights[3] += sgn * filter->sign[3];
        case 3: filter->weights[2] += sgn * filter->sign[2];
        case 2: filter->weights[1] += sgn * filter->sign[1];
        case 1: filter->weights[0] += sgn * filter->sign[0];
    }

    /* Move the past samples and past codes, and store the latest samples and codes. */
    memmove(&filter->history[1], &filter->history[0], (filter->taps - 1) * sizeof(int32_t));
    memmove(&filter->sign[1], &filter->sign[0], (filter->taps - 1) * sizeof(int8_t));
    filter->history[0] = sample;
    filter->sign[0] = SIGN(sample);
}