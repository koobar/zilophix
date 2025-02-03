#include "./include/zilophixdll.h"

error_code GetLastErrorCode() {
    return get_last_error_code();
}

void SetOnErrorExit(bool onErrorExit) {
    set_on_error_exit(onErrorExit);
}