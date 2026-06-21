//
// Created by 66 on 2026/6/21.
//

#include "logger.h"

#include <memory>

namespace utils {
    static void vprint(const char *prefix, const char *fmt, va_list args) {
        time_t now = time(nullptr);
        char timebuf[32];
        struct tm *tm_ptr = localtime(&now);
        strftime(timebuf, sizeof(timebuf), "%H:%M:%S", tm_ptr);

        printf("[%s] %s ", timebuf, prefix);
        vprintf(fmt, args);
        printf("\n");
    }

    void logger::info(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprint("INFO", fmt, args);
        va_end(args);
    }

    void logger::error(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprint("ERROR", fmt, args);
        va_end(args);
    }

    void logger::raw(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}
