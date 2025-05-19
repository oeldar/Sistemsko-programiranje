#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#define PATH_MAX 512

char *fd_to_path(int fd) {
    char proc_path[64];  // putanja do /proc/self/fd/<fd>
    snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);

    char *buf = malloc(PATH_MAX);
    if (!buf) {
        perror("malloc");
        return NULL;
    }

    ssize_t len = readlink(proc_path, buf, PATH_MAX - 1);
    if (len == -1) {
        perror("readlink");
        free(buf);
        return NULL;
    }

    buf[len] = '\0';  // ručno završiti string
    return buf;       // caller mora free(buf)
}
