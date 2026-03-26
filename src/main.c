#include "cli.h"
#include "compiler.h"
#include "config.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    CliArgs args;
    if (cli_parse(argc, argv, &args) != 0)
        return 1;

    /* --help */
    if (args.show_help)
    {
        cli_help();
        return 0;
    }

    /* --version */
    if (args.show_version)
    {
        cli_version();
        return 0;
    }

    /* Load config (defaults + file) */
    Config cfg;
    config_load(&cfg);

    /* Handle config subcommand */
    if (args.subcommand && strcmp(args.subcommand, "config") == 0)
    {
        if (!args.config_action)
        {
            fprintf(stderr, "runc: config requires an action (show, set, reset)\n");
            return 1;
        }

        if (strcmp(args.config_action, "show") == 0)
        {
            config_show(&cfg);
            return 0;
        }
        if (strcmp(args.config_action, "reset") == 0)
        {
            config_reset();
            printf("Config reset to defaults.\n");
            return 0;
        }
        if (strcmp(args.config_action, "set") == 0)
        {
            if (!args.config_key || !args.config_value)
            {
                fprintf(stderr, "runc: config set requires <key> <value>\n");
                return 1;
            }
            if (config_set(args.config_key, args.config_value) == 0)
                printf("Set %s = %s\n", args.config_key, args.config_value);
            else
                fprintf(stderr, "runc: failed to set config\n");
            return 0;
        }

        fprintf(stderr, "runc: unknown config action: %s\n", args.config_action);
        return 1;
    }

    /* From here we need a source file */
    if (!args.source_file)
    {
        fprintf(stderr, "runc: no input file. Use --help for usage.\n");
        return 1;
    }

    /* Check that source file exists */
    if (!file_exists(args.source_file))
    {
        fprintf(stderr, "runc: file not found: %s\n", args.source_file);
        return 1;
    }

    /* Apply CLI overrides to config */
    bool silent = args.silent || cfg.silent;
    bool timer  = args.timer  || cfg.timer;

    /* Determine compiler */
    const char *compiler = args.compiler;
    if (!compiler || !compiler[0])
        compiler = cfg.compiler[0] ? cfg.compiler : NULL;
    if (!compiler || !compiler[0])
    {
        compiler = compiler_detect();
        if (!compiler)
        {
            compiler_not_found_hint();
            return 1;
        }
    }

    /* Determine output path */
    char *output_path = NULL;
    char *bin_name = basename_no_ext(args.source_file);

    if (args.output)
    {
        output_path = strdup(args.output);
    }
    else if (args.visible)
    {
        /* Place binary next to source file */
        char *srcdir = dir_of(args.source_file);
        size_t len = strlen(srcdir) + strlen(bin_name) + 2;
        output_path = malloc(len);
        snprintf(output_path, len, "%s/%s", srcdir, bin_name);
        free(srcdir);
    }
    else
    {
        /* Hidden directory (default) */
        char *dir = expand_home(BIN_DIR);
        mkdirp(dir);
        size_t len = strlen(dir) + strlen(bin_name) + 2;
        output_path = malloc(len);
        snprintf(output_path, len, "%s/%s", dir, bin_name);
        free(dir);
    }

    /* Prepare compile options */
    CompileOpts opts = {
        .source       = args.source_file,
        .output       = output_path,
        .compiler     = compiler,
        .extra_flags  = args.flags,
        .config_flags = cfg.flags,
    };

    /* Dry run: just show the command */
    if (args.dry_run)
    {
        char *cmd = compiler_build_cmd(&opts);
        printf("[dry-run] %s\n", cmd);
        free(cmd);
        free(output_path);
        free(bin_name);
        return 0;
    }

    /* Compile */
    int compile_rc = compiler_compile(&opts, silent);
    if (compile_rc != 0)
    {
        if (!silent)
            fprintf(stderr, "[runc] compilation failed (exit code %d)\n", compile_rc);
        free(output_path);
        free(bin_name);
        return 1;
    }

    if (!silent)
    {
        printf("[runc] running: %s\n", output_path);
        fflush(stdout);
    }

    /* Run */
    long long t0 = 0;
    if (timer)
        t0 = time_ms();

    int run_rc = compiler_run(output_path);

    if (timer)
    {
        long long elapsed = time_ms() - t0;
        printf("\n[runc] execution time: %lld.%03lld s\n", elapsed / 1000, elapsed % 1000);
    }

    /* Cleanup binary */
    bool should_clean = cfg.auto_clean && !args.keep && !args.output;
    if (should_clean)
        unlink(output_path);

    free(output_path);
    free(bin_name);
    return run_rc;
}
