#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
  DIR* dir;
  dir = opendir(".");

  if (dir == NULL)
    printf("%s", strerror(errno));

  char path[256];
  struct stat statt;
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    snprintf(path, sizeof(path), "./%s", entry->d_name);
    if (stat(path, &statt) >= 0) {
      char* time = ctime(&statt.st_atim);
      printf("%s\t%s\t%s\n", S_ISREG(statt.st_mode) ? "-" : "d", entry->d_name, time);

    }
  }


  return 0;
}
