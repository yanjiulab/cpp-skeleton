#include "liblog.h"

int main() {
    puts("hello\n");

    log_set_level(LOG_LEVEL_DEBUG);
    log_enable_color();
    logd("colorful debug");
    logi("colorful info");
    logw("colorful warn");
    loge("colorful error");
    logc("colorful critical");
    return 0;
}