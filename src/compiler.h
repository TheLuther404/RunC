#ifndef COMPILER_H
#define COMPILER_H

#include "config.h"
#include <stdbool.h>

typedef struct {
    const char *source;      /* Path to the .c file */
    const char *output;      /* Path for the output binary */
    const char *compiler;    /* "gcc" or "clang" */
    const char *extra_flags; /* Extra compiler flags from CLI */
    const char *config_flags;/* Default flags from config */
} CompileOpts;

/* Auto-detect the best available compiler.
   Returns "gcc", "clang", or NULL if none found. */
const char *compiler_detect(void);

/* Build the full compiler command string.
   Caller must free the returned string. */
char *compiler_build_cmd(const CompileOpts *opts);

/* Compile the source file. Returns the compiler exit code. */
int compiler_compile(const CompileOpts *opts, bool silent);

/* Run the compiled binary. Returns the program exit code. */
int compiler_run(const char *binary_path);

/* Print a helpful message when no compiler is found. */
void compiler_not_found_hint(void);

#endif /* COMPILER_H */
