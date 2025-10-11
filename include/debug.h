#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <time.h>

#ifdef DEBUG

// prevent unused warning in non debug builds
__attribute__((unused)) static clock_t DebugStartTime = 0;

#define DEBUG_INIT() (DebugStartTime = clock())
#define DEBUG_ELAPSED() ((double)(clock() - DebugStartTime) / CLOCKS_PER_SEC)
#define DEBUG_MESSAGE(M, ...) printf("[+%.3fs] " M "\n", DEBUG_ELAPSED(), ##__VA_ARGS__)

#else
#define DEBUG_MESSAGE(M, ...)
#define DEBUG_INIT()
#endif

#define DEBUG_ERROR(M, ...) DEBUG_MESSAGE("[ERROR]" M, ##__VA_ARGS__)
#define DEBUG_WARNING(M, ...) DEBUG_MESSAGE("[WARNING]" M, ##__VA_ARGS__)
#define DEBUG_INFO(M, ...) DEBUG_MESSAGE("[INFO]" M, ##__VA_ARGS__)

#define DEBUG_APPLICATION_ERROR(M, ...) DEBUG_ERROR(" Application - " M, ##__VA_ARGS__)
#define DEBUG_COMPONENTS_ERROR(M, ...) DEBUG_ERROR(" Components - " M, ##__VA_ARGS__)
#define DEBUG_CORE_ERROR(M, ...) DEBUG_ERROR(" Core - " M, ##__VA_ARGS__)

#define DEBUG_APPLICATION_WARNING(M, ...) DEBUG_WARNING(" Application - " M, ##__VA_ARGS__)
#define DEBUG_COMPONENTS_WARNING(M, ...) DEBUG_WARNING(" Components - " M, ##__VA_ARGS__)
#define DEBUG_CORE_WARNING(M, ...) DEBUG_WARNING(" Core - " M, ##__VA_ARGS__)

#define DEBUG_APPLICATION_INFO(M, ...) DEBUG_INFO(" Application - " M, ##__VA_ARGS__)
#define DEBUG_COMPONENTS_INFO(M, ...) DEBUG_INFO(" Components - " M, ##__VA_ARGS__)
#define DEBUG_CORE_INFO(M, ...) DEBUG_INFO(" Core - " M, ##__VA_ARGS__)

#endif