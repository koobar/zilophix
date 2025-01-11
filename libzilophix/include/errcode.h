#ifndef ERROR_HEADER_INCLUDED
#define ERROR_HEADER_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#define ERROR_OK                                            0x0000      /* No error. */
#define ERROR_UNKNOWN                                       0xffff      /* Unknown error. */

#define ERROR_FILE_ACCESS_FAILED_TO_READ_UINT8              0x0010      /* Failed to read unsigned 8-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_UINT16             0x0011      /* Failed to read unsigned 16-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_UINT32             0x0012      /* Failed to read unsigned 32-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_INT16              0x0013      /* Failed to read signed 16-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_INT32              0x0014      /* Failed to read signed 32-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_CHAR               0x0015      /* Failed to read ASCII char. */
#define ERROR_FILE_ACCESS_FAILED_TO_READ_BOOL               0x0016      /* Failed to read boolean value. */

#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT8             0x0017      /* Failed to write unsigned 8-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT16            0x0018      /* Failed to write unsigned 16-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_UINT32            0x0019      /* Failed to write unsigned 32-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT16             0x001a      /* Failed to write signed 16-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_INT32             0x001b      /* Failed to write signed 32-bits integer. */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_CHAR              0x001c      /* Failed to write ASCII char. */
#define ERROR_FILE_ACCESS_FAILED_TO_WRITE_BOOL              0x001d      /* Failed to write boolean value. */

#define ERROR_BIT_STREAM_CANNOT_ALLOCATE_MEMORY             0x0020      /* Failed to allocate memory in bitstream. */
#define ERROR_BIT_STREAM_NOT_READ_MODE                      0x0021      /* Called bitstream read function in write mode. */
#define ERROR_BIT_STREAM_NOT_WRITE_MODE                     0x0022      /* Called bitstream write function in read mode. */

#define ERROR_LMS_CANNOT_ALLOCATE_MEMORY                    0x0030      /* Failed to allocate memory in SSLMS filter. */
#define ERROR_LMS_INVALID_FILTER_TAPS                       0x0031      /* SSLMS taps out of range error. */
#define ERROR_LMS_UNSUPPORTED_PCM_BITS                      0x0032      /* The signal to be processed by the SSLMS filter had an incompatible quantization bit rate. */

#define ERROR_POLYNOMIAL_PREDICTOR_CANNOT_ALLOCATE_MEMORY   0x0040      /* Failed to allocate memory in polynomial filter. */

#define ERROR_BLOCK_CANNOT_ALLOCATE_MEMORY                  0x0050      /* Failed to allocate memory in block. */

#define ERROR_SUB_BLOCK_CANNOT_ALLOCATE_MEMORY              0x0060      /* Failed to allocate memory in subblock. */

#define ERROR_RICE_CODING_INVALID_PARAMETER                 0x0070      /* Invalid entropy parameter detected. */

#define ERROR_DECODER_CANNOT_ALLOCATE_MEMORY                0x0080      /* Failed to allocate memory in decoder. */
#define ERROR_DECODER_FAILED_TO_OPEN_FILE                   0x0081      /* Failed to open file in decoder. */
#define ERROR_DECODER_INVALID_MAGIC_NUMBER                  0x0082      /* Invalid magic number detected in decoder. */
#define ERROR_DECODER_UNSUPPORTED_FORMAT_VERSION            0x0083      /* Unsupported ZilophiX format version detected in decoder. */

#define ERROR_ENCODER_CANNOT_ALLOCATE_MEMORY                0x0090      /* Failed to allocate memory in encoder. */
#define ERROR_ENCODER_FAILED_TO_OPEN_FILE                   0x0091      /* Failed to open file in encoder. */

typedef uint16_t error_code;

/*!
 * @brief           Reports error code.
 * @param error     Error code.
 */
void report_error(error_code error);

/*!
 * @brief           Get last reported error code
 * @return          Error code.
 */
error_code get_last_error_code();

/*!
 * @brief           Set on error exit.
 * @param value     If true, the program will immediately exit when an error occurs. If false, the program will not exit.
 */
void set_on_error_exit(bool value);

#endif