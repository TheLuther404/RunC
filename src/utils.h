#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

/* Return true if the file at `path` exists and is readable. */
bool file_exists(const char *path);

/* Return true if `cmd` is found in PATH. */
bool command_exists(const char *cmd);

/* Detect OS: 0 = macOS, 1 = Linux, -1 = unknown. */
int detect_os(void);

/* Expand a leading ~ to the user's home directory.
   Caller must free the returned string. */
char *expand_home(const char *path);

/* Create directories recursively (like mkdir -p). */
int mkdirp(const char *path);

/* Get the base name of a file without extension.
   Caller must free the returned string. */
char *basename_no_ext(const char *path);

/* Get the directory part of a path.
   Caller must free the returned string. */
char *dir_of(const char *path);

/* Get current time in milliseconds (monotonic). */
long long time_ms(void);

#endif /* UTILS_H */
