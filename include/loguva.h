#ifndef LOGUVA_H
#define LOGUVA_H

#include <stdbool.h>
#include <stdio.h>

#include "types.h"

#define loguva(lv, fmt, ...)                                                   \
  loguva_log(lv, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

enum log_levels { DEBUG, INFO, WARNING, ERROR, FATAL };

bool loguva_add_stream(FILE *stream);
FILE *loguva_add_file(const char *path);
bool loguva_log(enum log_levels lv, const char *path, u64 line, const char *fmt,
                ...);

#endif
