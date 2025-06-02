#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = 0;
    char path[1024];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);

    char real_path[1024];
    ssize_t len = readlink(path, real_path, sizeof(real_path) - 1);
    if (len != -1) {
        real_path[len] = '\0';
        printf("Fajl putanja: %s\n", real_path);
    } else {
        perror("readlink");
    }

    close(fd);
    return 0;
}

