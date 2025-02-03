#ifndef ZILOPHIXDLL_HEADER_INCLUDED
#define ZILOPHIXDLL_HEADER_INCLUDED

#include "errcode.h"
#include <stdbool.h>

/*!
 * @brief       Gets last error code.
 * @return      Error code
 */
error_code GetLastErrorCode();

/*!
 * @brief               Set on error exit.
 */
void SetOnErrorExit(bool onErrorExit);

#endif
