#include "./include/errcode.h"
#include <stdio.h>
#include <stdlib.h>

static bool on_error_exit = true;
static error_code last_error_code = ERROR_OK;

/*!
 * @brief           Reports error code.
 * @param error     Error code.
 */
void report_error(error_code error) {
    last_error_code = error;
    fprintf(stderr, "[Error] ERROR CODE = %#06x\n", error);

    if (on_error_exit) {    
        exit((int)error);
    }
}

/*!
 * @brief           Get last reported error code
 * @return          Error code.
 */
error_code get_last_error_code() {
    return last_error_code;
}

/*!
 * @brief           Set on error exit.
 * @param value     If true, the program will immediately exit when an error occurs. If false, the program will not exit.
 */
void set_on_error_exit(bool value) {
    on_error_exit = value;
}
