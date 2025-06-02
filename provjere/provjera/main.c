#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char* fileName = argv[1];
  char* content = argv[2];
  char* pos = argv[3];
  char* perm = argv[4];

  printf("%s, %s, %s, %s\n", fileName, content, pos, perm);

  int position, mode;
  char* end;
  position = strtol(pos, &end, 10);
  mode = strtol(perm, &end, 8);

  if (position > 512) {
    printf("Position overflow, position set to 0.\n");
    position = 0;
  }

  printf("%d, %d", position, mode);
  int fd = open(fileName, O_CREAT | O_WRONLY, mode);
  lseek(fd, position, SEEK_SET);
  write(fd, content, strlen(content));

  close(fd);


  return 0;
}
