#ifndef CLI_H
#define CLI_H

#include "config.h"
#include <stdbool.h>

typedef struct {
    /* Source file to compile */
    const char *source_file;

    /* Overrides from CLI flags */
    bool   keep;           /* --keep: don't delete binary */
    bool   visible;        /* --visible: place binary next to source */
    bool   hidden_flag;    /* --hidden: force hidden dir */
    bool   silent;         /* --silent */
    bool   timer;          /* --timer */
    bool   dry_run;        /* --dry-run */
    bool   show_help;      /* --help */
    bool   show_version;   /* --version */

    const char *output;    /* --output <path> */
    const char *compiler;  /* --compiler <gcc|clang> */
    const char *flags;     /* --flags "<flags>" */

    /* Subcommand: "config" or NULL */
    const char *subcommand;
    /* Config sub-args: "show", "set", "reset" */
    const char *config_action;
    const char *config_key;
    const char *config_value;
} CliArgs;

/* Parse argc/argv into CliArgs. Returns 0 on success, -1 on error. */
int cli_parse(int argc, char **argv, CliArgs *args);

/* Print help message. */
void cli_help(void);

/* Print version. */
void cli_version(void);

#endif /* CLI_H */
