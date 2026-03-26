#include "cli.h"
#include "config.h"

#include <stdio.h>
#include <string.h>

void cli_help(void)
{
    printf(
        "runc v" RUNC_VERSION " - Compile and run C files instantly\n"
        "\n"
        "USAGE:\n"
        "  runc <file.c> [flags]\n"
        "  rc   <file.c> [flags]    (alias)\n"
        "\n"
        "FLAGS:\n"
        "  -h, --help               Show this help message\n"
        "  -v, --version            Show version\n"
        "  -k, --keep               Keep the compiled binary after execution\n"
        "  -o, --output <path>      Custom output path for the binary\n"
        "  -H, --hidden             Place binary in ~/.runc/bin/ (default)\n"
        "  -V, --visible            Place binary next to the source file\n"
        "  -c, --compiler <name>    Use a specific compiler (gcc or clang)\n"
        "  -f, --flags \"<flags>\"    Pass extra flags to the compiler\n"
        "  -s, --silent             Suppress runc output, show only program output\n"
        "  -t, --timer              Show execution time after the program finishes\n"
        "  -d, --dry-run            Show the compile command without executing\n"
        "\n"
        "SUBCOMMANDS:\n"
        "  runc config show         Print current configuration\n"
        "  runc config set <k> <v>  Set a config value\n"
        "  runc config reset        Restore default configuration\n"
        "\n"
        "EXAMPLES:\n"
        "  runc main.c              Compile and run main.c\n"
        "  runc main.c -k           Keep the binary after running\n"
        "  runc main.c -t -f \"-O2\"  Timed run with optimization\n"
        "  runc main.c -V           Binary next to source file\n"
        "  runc main.c -d           Dry run (show command only)\n"
        "  runc config set timer true\n"
        "\n"
        "CONFIG FILE: ~/.runc/config\n"
    );
}

void cli_version(void)
{
    printf("runc v%s\n", RUNC_VERSION);
}

int cli_parse(int argc, char **argv, CliArgs *args)
{
    memset(args, 0, sizeof(*args));

    if (argc < 2)
    {
        fprintf(stderr, "runc: no input file. Use --help for usage.\n");
        return -1;
    }

    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];

        /* Subcommand: config */
        if (strcmp(arg, "config") == 0 && i == 1)
        {
            args->subcommand = "config";
            if (i + 1 < argc)
            {
                args->config_action = argv[++i];

                if (strcmp(args->config_action, "set") == 0)
                {
                    if (i + 2 < argc)
                    {
                        args->config_key   = argv[++i];
                        args->config_value = argv[++i];
                    }
                    else
                    {
                        fprintf(stderr, "runc: config set requires <key> <value>\n");
                        return -1;
                    }
                }
            }
            else
            {
                fprintf(stderr, "runc: config requires an action (show, set, reset)\n");
                return -1;
            }
            continue;
        }

        /* Long flags */
        if (strcmp(arg, "--help") == 0)    { args->show_help = true;    continue; }
        if (strcmp(arg, "--version") == 0) { args->show_version = true; continue; }
        if (strcmp(arg, "--keep") == 0)    { args->keep = true;         continue; }
        if (strcmp(arg, "--hidden") == 0)  { args->hidden_flag = true;  continue; }
        if (strcmp(arg, "--visible") == 0) { args->visible = true;      continue; }
        if (strcmp(arg, "--silent") == 0)  { args->silent = true;       continue; }
        if (strcmp(arg, "--timer") == 0)   { args->timer = true;        continue; }
        if (strcmp(arg, "--dry-run") == 0) { args->dry_run = true;      continue; }

        if (strcmp(arg, "--output") == 0)
        {
            if (++i < argc) { args->output = argv[i]; continue; }
            fprintf(stderr, "runc: --output requires a path\n");
            return -1;
        }
        if (strcmp(arg, "--compiler") == 0)
        {
            if (++i < argc) { args->compiler = argv[i]; continue; }
            fprintf(stderr, "runc: --compiler requires a value\n");
            return -1;
        }
        if (strcmp(arg, "--flags") == 0)
        {
            if (++i < argc) { args->flags = argv[i]; continue; }
            fprintf(stderr, "runc: --flags requires a value\n");
            return -1;
        }

        /* Short flags */
        if (arg[0] == '-' && arg[1] != '-' && strlen(arg) == 2)
        {
            switch (arg[1])
            {
                case 'h': args->show_help = true;    continue;
                case 'v': args->show_version = true; continue;
                case 'k': args->keep = true;         continue;
                case 'H': args->hidden_flag = true;  continue;
                case 'V': args->visible = true;      continue;
                case 's': args->silent = true;        continue;
                case 't': args->timer = true;         continue;
                case 'd': args->dry_run = true;       continue;
                case 'o':
                    if (++i < argc) { args->output = argv[i]; continue; }
                    fprintf(stderr, "runc: -o requires a path\n");
                    return -1;
                case 'c':
                    if (++i < argc) { args->compiler = argv[i]; continue; }
                    fprintf(stderr, "runc: -c requires a value\n");
                    return -1;
                case 'f':
                    if (++i < argc) { args->flags = argv[i]; continue; }
                    fprintf(stderr, "runc: -f requires a value\n");
                    return -1;
                default:
                    fprintf(stderr, "runc: unknown flag: %s\n", arg);
                    return -1;
            }
        }

        /* Positional argument: source file */
        if (!args->source_file && arg[0] != '-')
        {
            args->source_file = arg;
            continue;
        }

        fprintf(stderr, "runc: unknown argument: %s\n", arg);
        return -1;
    }

    return 0;
}
