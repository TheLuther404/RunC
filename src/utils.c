#include "utils.h"

#include <errno.h>
#include <libgen.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

bool file_exists(const char *path)
{
    return access(path, F_OK) == 0;
}

bool command_exists(const char *cmd)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "command -v %s >/dev/null 2>&1", cmd);
    return system(buf) == 0;
}

int detect_os(void)
{
#if defined(__APPLE__) && defined(__MACH__)
    return 0; /* macOS */
#elif defined(__linux__)
    return 1; /* Linux */
#else
    return -1;
#endif
}

char *expand_home(const char *path)
{
    if (!path || path[0] != '~')
    {
        return strdup(path ? path : "");
    }

    const char *home = getenv("HOME");
    if (!home)
    {
        struct passwd *pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }

    size_t len = strlen(home) + strlen(path); /* path includes '~' */
    char *result = malloc(len + 1);
    if (!result) return NULL;

    snprintf(result, len + 1, "%s%s", home, path + 1);
    return result;
}

int mkdirp(const char *path)
{
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);

    for (char *p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
                return -1;
            *p = '/';
        }
    }
    return mkdir(tmp, 0755) == 0 || errno == EEXIST ? 0 : -1;
}

char *basename_no_ext(const char *path)
{
    /* Work on a copy because basename may modify its argument */
    char *copy = strdup(path);
    if (!copy) return NULL;

    char *base = basename(copy);
    char *result = strdup(base);
    free(copy);
    if (!result) return NULL;

    /* Strip the last extension */
    char *dot = strrchr(result, '.');
    if (dot && dot != result)
        *dot = '\0';

    return result;
}

char *dir_of(const char *path)
{
    char *copy = strdup(path);
    if (!copy) return NULL;

    char *d = dirname(copy);
    char *result = strdup(d);
    free(copy);
    return result;
}

long long time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
