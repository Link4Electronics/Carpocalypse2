/*
 * Case-insensitive path resolution for Linux.
 *
 * The retail game was built for Windows where file names are case-insensitive,
 * and it requests data files in their original upper-case spelling ("DATA",
 * "GENERAL.TXT", ...). On Linux this fails when the data tree uses lower-case
 * names. This module resolves each path component against the real directory
 * contents, accepting any letter casing.
 *
 * `__wrap_fopen` intercepts every fopen() call in the binary (see the
 * `-Wl,--wrap=fopen` link option): the plain open is attempted first so
 * correct-case paths behave exactly as before, and only on failure do we walk
 * the directory chain looking for a case-insensitive match.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

const char* carpocalypse2_fix_path_case(const char* pPath) {
    static char fixed[1024];
    char walk[1024];
    char* component;

    if (pPath == NULL || strlen(pPath) >= sizeof(walk)) {
        return pPath;
    }
    strcpy(walk, pPath);
    fixed[0] = '\0';
    if (walk[0] == '/') {
        strcat(fixed, "/");
    }

    component = strtok(walk, "/");
    while (component != NULL) {
        DIR* dir;
        struct dirent* entry;
        char match[512];
        int found;

        dir = opendir(fixed[0] == '\0' ? "." : fixed);
        if (dir == NULL) {
            /* Cannot inspect this level; keep the rest verbatim. */
            if (fixed[0] != '\0' && fixed[strlen(fixed) - 1] != '/') {
                strcat(fixed, "/");
            }
            strcat(fixed, component);
            component = strtok(NULL, "/");
            continue;
        }
        match[0] = '\0';
        found = 0;
        while ((entry = readdir(dir)) != NULL) {
            if (strcasecmp(entry->d_name, component) == 0) {
                strncpy(match, entry->d_name, sizeof(match) - 1);
                match[sizeof(match) - 1] = '\0';
                found = 1;
                break;
            }
        }
        closedir(dir);

        if (fixed[0] != '\0' && fixed[strlen(fixed) - 1] != '/') {
            strcat(fixed, "/");
        }
        if (found) {
            strcat(fixed, match);
        } else {
            strcat(fixed, component);
        }
        component = strtok(NULL, "/");
    }

    return fixed;
}

extern FILE* __real_fopen(const char* pPath, const char* pMode);

FILE* __wrap_fopen(const char* pPath, const char* pMode) {
    FILE* f;

    f = __real_fopen(pPath, pMode);
    if (f == NULL) {
        const char* fixed = carpocalypse2_fix_path_case(pPath);
        f = __real_fopen(fixed, pMode);
        if (getenv("CARPOCALYPSE2_FOPEN_DEBUG") != NULL) {
            fprintf(stderr, "[fopen MISS] %s%s\n", pPath, (f != NULL) ? "  (case-fixed OK)" : "");
            if (f == NULL && fixed != pPath) {
                fprintf(stderr, "[fopen miss2] %s\n", fixed);
            }
        }
    }
    return f;
}

/* The S3 sound layer opens data files with open() rather than fopen(), so we
 * wrap open()/open64() through the same case-insensitive resolver. */
extern int __real_open(const char* pPath, int pFlags, ...);
extern int __real_open64(const char* pPath, int pFlags, ...);

static int case_fix_open(const char* pPath, int pFlags, mode_t pMode) {
    int fd;

    fd = (int)__real_open(pPath, pFlags, pMode);
    if (fd < 0) {
        const char* fixed = carpocalypse2_fix_path_case(pPath);
        fd = (int)__real_open(fixed, pFlags, pMode);
        if (getenv("CARPOCALYPSE2_FOPEN_DEBUG") != NULL) {
            fprintf(stderr, "[open MISS] %s%s\n", pPath, (fd >= 0) ? "  (case-fixed OK)" : "");
        }
    }
    return fd;
}

int __wrap_open(const char* pPath, int pFlags, ...) {
    mode_t mode = 0;
    va_list ap;

    if ((pFlags & O_CREAT) != 0) {
        va_start(ap, pFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return case_fix_open(pPath, pFlags, mode);
}

int __wrap_open64(const char* pPath, int pFlags, ...) {
    mode_t mode = 0;
    va_list ap;

    if ((pFlags & O_CREAT) != 0) {
        va_start(ap, pFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return case_fix_open(pPath, pFlags, mode);
}
