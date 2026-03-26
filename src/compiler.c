#include "compiler.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

const char *compiler_detect(void)
{
    if (command_exists("gcc"))   return "gcc";
    if (command_exists("clang")) return "clang";
    return NULL;
}

char *compiler_build_cmd(const CompileOpts *opts)
{
    char *cmd = malloc(2048);
    if (!cmd) return NULL;

    int n = snprintf(cmd, 2048, "%s", opts->compiler);

    /* Config default flags */
    if (opts->config_flags && opts->config_flags[0])
        n += snprintf(cmd + n, 2048 - n, " %s", opts->config_flags);

    /* Extra CLI flags */
    if (opts->extra_flags && opts->extra_flags[0])
        n += snprintf(cmd + n, 2048 - n, " %s", opts->extra_flags);

    /* Output and source */
    snprintf(cmd + n, 2048 - n, " -o \"%s\" \"%s\"", opts->output, opts->source);

    return cmd;
}

int compiler_compile(const CompileOpts *opts, bool silent)
{
    char *cmd = compiler_build_cmd(opts);
    if (!cmd)
    {
        fprintf(stderr, "runc: memory allocation failed\n");
        return -1;
    }

    if (!silent)
    {
        printf("[runc] compiling: %s\n", cmd);
        fflush(stdout);
    }

    int status = system(cmd);
    free(cmd);

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return -1;
}

int compiler_run(const char *binary_path)
{
    int status = system(binary_path);

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return -1;
}

void compiler_not_found_hint(void)
{
    fprintf(stderr, "runc: no C compiler found.\n");

    int os = detect_os();
    if (os == 0)
        fprintf(stderr, "  Install one with: brew install gcc\n");
    else if (os == 1)
        fprintf(stderr, "  Install one with: sudo apt install gcc\n");
    else
        fprintf(stderr, "  Please install gcc or clang.\n");
}
