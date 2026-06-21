//
// Created by 66 on 2026/6/21.
//

#ifndef MINIRTMP_LOGGER_H
#define MINIRTMP_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace utils {
    class logger {
    public:
        static void info(const char *fmt, ...);

        static void error(const char *fmt, ...);

        static void raw(const char *fmt, ...);
    };
}


#endif //MINIRTMP_LOGGER_H
