#ifndef EVLOGGING_H
#define LOGGING_H

#include <stdio.h>

// Define log levels
#define EVLOG_LEVEL_DEBUG 0
#define EVLOG_LEVEL_INFO  1
#define EVLOG_LEVEL_ERROR 2

#ifndef EVLOG_LEVEL
#define EVLOG_LEVEL LOG_LEVEL_ERROR
#endif

// Logging macros
#define EVLOG_DEBUG(fmt, ...) \
    do { if (EVLOG_LEVEL <= EVLOG_LEVEL_DEBUG) printf("[DEBUG] %s: " fmt "\n", __func__, ##__VA_ARGS__); } while (0)

#define EVLOG_INFO(fmt, ...) \
    do { if (EVLOG_LEVEL <= EVLOG_LEVEL_INFO) printf("[INFO] %s: " fmt "\n", __func__, ##__VA_ARGS__); } while (0)

#define EVLOG_ERROR(fmt, ...) \
    do { if (EVLOG_LEVEL <= EVLOG_LEVEL_ERROR) printf("[ERROR] %s: " fmt "\n", __func__, ##__VA_ARGS__); } while (0)

#endif // LOGGING_H
