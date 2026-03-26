#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define RUNC_VERSION "1.0.0"
#define CONFIG_DIR   "~/.runc"
#define CONFIG_PATH  "~/.runc/config"
#define BIN_DIR      "~/.runc/bin"

typedef struct {
    bool   hidden;        /* Place binary in hidden dir (default: true) */
    bool   auto_clean;    /* Delete binary after execution (default: true) */
    char   compiler[32];  /* "gcc" or "clang" */
    char   flags[256];    /* Default compiler flags */
    bool   timer;         /* Show execution time */
    bool   silent;        /* Suppress runc output */
} Config;

/* Load config from ~/.runc/config. Fills `cfg` with defaults first. */
void config_load(Config *cfg);

/* Save the current config to ~/.runc/config. */
int config_save(const Config *cfg);

/* Reset config file to defaults. */
int config_reset(void);

/* Set a single key=value in the config. */
int config_set(const char *key, const char *value);

/* Print the current config to stdout. */
void config_show(const Config *cfg);

/* Fill `cfg` with default values. */
void config_defaults(Config *cfg);

#endif /* CONFIG_H */
