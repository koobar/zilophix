#ifndef ZILOPHIXDLL_HEADER_INCLUDED
#define ZILOPHIXDLL_HEADER_INCLUDED

#include "errcode.h"

/*!
 * @brief       最後に発生したエラーのエラーコードを取得します。
 * @return      最後に発生したエラーのエラーコード
 */
error_code GetLastErrorCode();

#endif
