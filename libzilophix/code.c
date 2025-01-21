#include "./include/macro.h"
#include "./include/code.h"
#include "./include/errcode.h"
#include "./include/sub_block.h"
#include "./include/zilophix.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define RESTORE_PARTITION_COUNT(partition_parameter)            (LSHIFT(1, partition_parameter))
#define RESTORE_PARTITION_SIZE(data_size, partition_parameter)  ((data_size) / RESTORE_PARTITION_COUNT(partition_parameter))

#define MINIMUM_PARTITION_PARAMETER_FOR_VER_1_0     1
#define MINIMUM_PARTITION_PARAMETER_FOR_VER_1_1     0
#define PARTITION_PARAMETER_NEED_BITS               2

#define RICE_PARAMETER_MAX_FOR_PCM16            14
#define BLANK_PARAMETER_FOR_PCM16               15
#define ENTROPY_PARAMETER_NEED_BITS_FOR_PCM16   4

#define RICE_PARAMETER_MAX_FOR_PCM24            30
#define BLANK_PARAMETER_FOR_PCM24               31
#define ENTROPY_PARAMETER_NEED_BITS_FOR_PCM24   5

#ifdef _MSC_VER
#define __builtin_clz(x) clz(x)

static inline uint32_t popcnt(uint32_t x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;
}

static inline uint32_t clz(uint32_t x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return 32 - popcnt(x);
}

#endif

/*!
 * @brief                   Calculates the number of bits required to Rice encode the given data with the specified parameters.
 * @param *data             Pointer to data
 * @param data_size         Size of data.
 * @param parameter         Rice parameter
 * @return                  The number of bits required to Rice-encode the specified data with the specified parameters.
 */
static uint32_t compute_rice_total_bits(const int32_t* data, const uint16_t start, const uint16_t data_size, const uint32_t parameter) {
    uint32_t estimated_size = 0;
    uint32_t val;
    uint32_t quotient;
    uint16_t offset;

    for (offset = 0; offset < data_size; ++offset) {
        val = CONVERT_INT32_TO_UINT32(data[start + offset]);
        quotient = RSHIFT(val, parameter);

        estimated_size += quotient;
        estimated_size += 1;
    }

    estimated_size += (parameter * data_size);

    return estimated_size;
}

/*!
 * @brief               Calculates the optimal Rice coding parameters for the given data.
 * @param *coder        Pointer to code.
 * @param *data         Pointer to data.
 * @param start         Offset of data begin.
 * @param data_size     Data size.
 * @param *total_bits   [OUT] The total number of bits of data when Rice-encoded with the obtained parameters
 * @return              The optimal Rice encoding parameters for the given data
 */
static uint32_t compute_optimal_rice_parameter(code* coder, const int32_t* data, uint16_t start, uint16_t data_size, uint32_t* total_bits) {
    double sum = 0;
    double mean = 0;
    uint32_t i, imean, parameter;

    /* Calculate the average of the absolute values ​​of the data that appears. */
    for (i = 0; i < data_size; ++i) {
        sum += abs(data[start + i]);
    }
    mean = sum / data_size;

    /* The number of bits required to express the average rounded to an integer is taken as a parameter. */
    imean = CLAMP((uint32_t)floor(mean), 0, UINT32_MAX);
    if (imean == 0){
        /* If the average is zero, it is OK to output the remainder as zero bits (= no output). */
        parameter = 0;
    }
    else{
        /* If the mean is non-zero, the optimal parameters can be estimated as (32 - the minimum number of bits required to represent the mean) */
        parameter = CLAMP(32 - __builtin_clz(imean), 0, coder->rice_parameter_max);
    }
    
    *(total_bits) = compute_rice_total_bits(data, start, data_size, parameter);
    return parameter;
}

/*!
 * @brief                   Calculates the entropy coding parameters that should be used on the partition.
 * @param *coder            Pointer to code.
 * @param *data             Pointer to data.
 * @param start             Offset of partition begin.
 * @param partition_size    Size of partition.
 * @param *partition_bits   [OUT] The number of bits required for the entire partition when it is entropy coded using the calculated entropy coding parameters.
 * @return                  Optimal entropy coding parameter.
 */
static uint32_t compute_optimal_entropy_parameter(code* coder, const int32_t* data, uint16_t start, uint16_t partition_size, uint32_t* partition_bits) {
    bool is_blank_partition = true;
    uint16_t offset;

    /* Check blank partition. */
    for (offset = 0; offset < partition_size; ++offset) {
        if (data[offset + start] != 0) {
            is_blank_partition = false;
            break;
        }
    }

    if (is_blank_partition) {
        *(partition_bits) = coder->bits_of_entropy_parameter;
        return coder->blank_partition_parameter;
    }

    return compute_optimal_rice_parameter(coder, data, start, partition_size, partition_bits);
}

/*!
 * @brief               For the given data, it computes the optimal partition parameters as well as the entropy coding parameters to use for each partition.
 * @param *coder        Pointer to code.
 * @param *data         Pointer to data.
 * @param data_size     Size of data.
 * @return              Optimal partition parameter.
 */
static uint32_t compute_optimal_partition_parameter(code* coder, const int32_t* data, uint16_t data_size) {
    uint32_t partition_size;
    uint32_t partition_index;
    uint32_t trial_pp;
    uint32_t partition_count;
    uint32_t optimal_partition_parameter;
    uint32_t size, min_size;
    uint16_t start;
    uint32_t partition_bits;

    min_size = UINT32_MAX;
    optimal_partition_parameter = 0;

    for (trial_pp = coder->minimum_partition_parameter; trial_pp <= coder->maximum_partition_parameter; ++trial_pp) {
        partition_count = RESTORE_PARTITION_COUNT(trial_pp);
        partition_size = data_size / partition_count;
        size = 0;

        /* Calculate the optimal entropy coding parameters for each partition and store them in the work memory. */
        for (partition_index = 0; partition_index < partition_count; ++partition_index) {
            start = partition_size * partition_index;

            /* Save the entropy coding parameters of this partition in the working memory. */
            coder->workB[partition_index] = compute_optimal_entropy_parameter(coder, data, start, partition_size, &partition_bits);
            
            /* Update partition size. */
            size += partition_bits;
        }

        /* To the size of the partition, add the number of bits required to store the entropy coding parameters. */
        size += coder->bits_of_partition_parameter;

        /* If the partition size based on the parameters tried this time is smaller than the 
           minimum size of partitions already found, the minimum size is replaced and the entropy 
           coding parameters of all partitions stored in the working area are copied to the output area.*/
        if (min_size > size) {
            min_size = size;
            optimal_partition_parameter = trial_pp;
            memcpy(coder->workA, coder->workB, sizeof(uint32_t) * coder->maximum_partition_count);
        }
    }

    return optimal_partition_parameter;
}

/*!
 * @brief                   Write residuals to bitstream as rice code.
 * @param *stream           Pointer to bitstream.
 * @param *data             Pointer to residuals.
 * @param start             Offset of partition.
 * @param partition_size    Size of partition.
 * @param parameter         Rice parameter.
 */
static void write_rice_partition(bit_stream* stream, const int32_t* data, uint16_t start, uint16_t partition_size, uint32_t parameter) {
    uint16_t offset;

    for (offset = 0; offset < partition_size; ++offset) {
        bit_stream_write_rice_signed(stream, data[offset + start], parameter);
    }
}

/*!
 * @brief                   Read the prediction residuals contained in the Rice-coded partition.
 * @param *stream           Pointer to bitstream.
 * @param *data             Pointer to data.
 * @param start             Offset of partition
 * @param partition_size    Size of partition
 * @param parameter         Rice parameter
 */
static void read_rice_partition(bit_stream* stream, int32_t* data, uint16_t start, uint16_t partition_size, uint32_t parameter) {
    uint16_t offset;
    int32_t* pdata = data + start;

    for (offset = 0; offset < partition_size; ++offset) {
        *(pdata++) = bit_stream_read_rice_signed(stream, parameter);
    }
}

/*!
 * @brief               Writes the specified sub-block to the given bitstream using the Rice encoding.
 * @param *coder        Pointer to coder.
 * @param *sub_block    Pointer to subblock.
 */
static void write_sub_block(code* coder, const sub_block* sub_block) {
    uint32_t p, start;
    uint32_t parameter;
    uint16_t partition_size;
    uint32_t partition_parameter;
    uint32_t partition_count;

    /* Calculate partition parameter. */
    partition_parameter = compute_optimal_partition_parameter(coder, sub_block->samples, sub_block->size);

    /* Compute the number of partitions and partition size from parameter. */
    partition_count = RESTORE_PARTITION_COUNT(partition_parameter);
    partition_size = sub_block->size / partition_count;

    /* Write partition parameter. */
    bit_stream_write_uint(coder->bitstream, partition_parameter - coder->minimum_partition_parameter, coder->bits_of_partition_parameter);

    /* Write all subblocks. */
    for (p = 0; p < partition_count; ++p) {
        start = partition_size * p;
        parameter = coder->workA[p];

        /* Write parameter of entropy coding. */
        bit_stream_write_uint(coder->bitstream, (uint32_t)(parameter), coder->bits_of_entropy_parameter);

        /* If the entropy parameter is Rice parameter, write the partitions as Rice codes.*/
        if (parameter <= coder->rice_parameter_max) {
            write_rice_partition(coder->bitstream, sub_block->samples, start, partition_size, parameter);
        }
    }
}

/*!
 * @brief               Read Rice encoded subblock.
 * @param *coder        Pointer to code.
 * @param *sub_block    Pointer to subblock.
 */
static void read_sub_block(code* coder, sub_block* sub_block) {
    uint32_t parameter;
    uint16_t offset;
    uint32_t p, start;
    uint16_t partition_size;
    uint32_t partition_parameter;
    uint32_t partition_count;

    /* Read partition parameter. */
    partition_parameter = bit_stream_read_uint(coder->bitstream, coder->bits_of_partition_parameter) + coder->minimum_partition_parameter;

    /* Compute the number of partitions and partition size from parameter. */
    partition_count = RESTORE_PARTITION_COUNT(partition_parameter);
    partition_size = sub_block->size / partition_count;

    /* Read all partitions. */
    for (p = 0; p < partition_count; ++p) {
        start = p * partition_size;

        /* Read parameter of entropy coding. */
        parameter = bit_stream_read_uint(coder->bitstream, coder->bits_of_entropy_parameter);

        if (parameter <= coder->rice_parameter_max){
            read_rice_partition(coder->bitstream, sub_block->samples, start, partition_size, parameter);
        }
        else {
            for (offset = 0; offset < partition_size; ++offset) {
                sub_block->samples[start + offset] = 0;
            }
        }
    }
}

/*!
 * @brief           Create new instance of code.
 * @param *stream   Pointer to bit_stream.
 * @return          Pointer to created instance.
 */
code* code_create(bit_stream* stream, uint8_t fmt_version, uint8_t bits_per_sample) {
    code* result = (code*)malloc(sizeof(code));

    if (result == NULL) {
        return NULL;
    }
    
    if (bits_per_sample == 16) {
        result->rice_parameter_max = RICE_PARAMETER_MAX_FOR_PCM16;
        result->blank_partition_parameter = BLANK_PARAMETER_FOR_PCM16;
        result->bits_of_entropy_parameter = ENTROPY_PARAMETER_NEED_BITS_FOR_PCM16;
    }
    else if (bits_per_sample == 24) {
        result->rice_parameter_max = RICE_PARAMETER_MAX_FOR_PCM24;
        result->blank_partition_parameter = BLANK_PARAMETER_FOR_PCM24;
        result->bits_of_entropy_parameter = ENTROPY_PARAMETER_NEED_BITS_FOR_PCM24;
    }

    switch (fmt_version) {
    case FORMAT_VERSION_1_0:
        result->minimum_partition_parameter = MINIMUM_PARTITION_PARAMETER_FOR_VER_1_0;
        break;
    case FORMAT_VERSION_1_1:
        result->minimum_partition_parameter = MINIMUM_PARTITION_PARAMETER_FOR_VER_1_1;
        break;
    default:
        report_error(ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION);
        break;
    }

    result->bitstream = stream;
    result->bits_per_sample = bits_per_sample;
    result->maximum_partition_parameter = result->minimum_partition_parameter + 3;
    result->maximum_partition_count = RESTORE_PARTITION_COUNT(result->maximum_partition_parameter);
    result->bits_of_partition_parameter = PARTITION_PARAMETER_NEED_BITS;
    result->workA = (uint32_t*)calloc(result->maximum_partition_count, sizeof(uint32_t));
    result->workB = (uint32_t*)calloc(result->maximum_partition_count, sizeof(uint32_t));

    return result;
}

/*!
 * @brief           Release specified instance.
 * @param *coder    Pointer to code.
 */
void code_free(code* coder) {
    free(coder->workA);
    free(coder->workB);
}

/*!
 * @brief           Write block to bitstream.
 * @param *coder    Pointer to code.
 * @param *block    Pointer to block.
 */
void code_write_block(code* coder, const block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        write_sub_block(coder, block->sub_blocks[ch]);
    }
}

/*!
 * @brief           Read block from bitstream.
 * @param *coder    Pointer to code.
 * @param *block    Pointer to block.
 */
void code_read_block(code* coder, block* block) {
    uint8_t ch;

    for (ch = 0; ch < block->num_channels; ++ch) {
        read_sub_block(coder, block->sub_blocks[ch]);
    }
}