#include "config.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void config_defaults(Config *cfg)
{
    cfg->hidden     = true;
    cfg->auto_clean = true;
    strncpy(cfg->compiler, "", sizeof(cfg->compiler) - 1);
    strncpy(cfg->flags, "", sizeof(cfg->flags) - 1);
    cfg->timer  = false;
    cfg->silent = false;
}

/* Parse a single key=value line into the config struct. */
static void config_parse_line(Config *cfg, const char *key, const char *value)
{
    if (strcmp(key, "hidden") == 0)
        cfg->hidden = strcmp(value, "true") == 0;
    else if (strcmp(key, "auto_clean") == 0)
        cfg->auto_clean = strcmp(value, "true") == 0;
    else if (strcmp(key, "default_compiler") == 0)
        strncpy(cfg->compiler, value, sizeof(cfg->compiler) - 1);
    else if (strcmp(key, "default_flags") == 0)
        strncpy(cfg->flags, value, sizeof(cfg->flags) - 1);
    else if (strcmp(key, "timer") == 0)
        cfg->timer = strcmp(value, "true") == 0;
    else if (strcmp(key, "silent") == 0)
        cfg->silent = strcmp(value, "true") == 0;
}

void config_load(Config *cfg)
{
    config_defaults(cfg);

    char *path = expand_home(CONFIG_PATH);
    if (!path) return;

    FILE *fp = fopen(path, "r");
    free(path);
    if (!fp) return;

    char line[512];
    while (fgets(line, sizeof(line), fp))
    {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n')
            continue;

        /* Remove trailing newline */
        line[strcspn(line, "\n")] = '\0';

        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        const char *key   = line;
        const char *value = eq + 1;

        config_parse_line(cfg, key, value);
    }

    fclose(fp);
}

static int config_write(const Config *cfg, const char *filepath)
{
    char *dir = expand_home(CONFIG_DIR);
    if (!dir) return -1;
    mkdirp(dir);
    free(dir);

    char *path = expand_home(filepath);
    if (!path) return -1;

    FILE *fp = fopen(path, "w");
    free(path);
    if (!fp)
    {
        perror("runc: cannot write config");
        return -1;
    }

    fprintf(fp, "# runc configuration\n");
    fprintf(fp, "hidden=%s\n",     cfg->hidden ? "true" : "false");
    fprintf(fp, "auto_clean=%s\n", cfg->auto_clean ? "true" : "false");
    fprintf(fp, "default_compiler=%s\n", cfg->compiler);
    fprintf(fp, "default_flags=%s\n",    cfg->flags);
    fprintf(fp, "timer=%s\n",      cfg->timer ? "true" : "false");
    fprintf(fp, "silent=%s\n",     cfg->silent ? "true" : "false");

    fclose(fp);
    return 0;
}

int config_save(const Config *cfg)
{
    return config_write(cfg, CONFIG_PATH);
}

int config_reset(void)
{
    Config cfg;
    config_defaults(&cfg);
    return config_save(&cfg);
}

int config_set(const char *key, const char *value)
{
    Config cfg;
    config_load(&cfg);
    config_parse_line(&cfg, key, value);
    return config_save(&cfg);
}

void config_show(const Config *cfg)
{
    printf("hidden           = %s\n", cfg->hidden ? "true" : "false");
    printf("auto_clean       = %s\n", cfg->auto_clean ? "true" : "false");
    printf("default_compiler = %s\n", cfg->compiler[0] ? cfg->compiler : "(auto-detect)");
    printf("default_flags    = %s\n", cfg->flags[0] ? cfg->flags : "(none)");
    printf("timer            = %s\n", cfg->timer ? "true" : "false");
    printf("silent           = %s\n", cfg->silent ? "true" : "false");
}
