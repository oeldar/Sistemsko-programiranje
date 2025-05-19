#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char* fileName = argv[1];
  char* content = argv[2];
  char* poz = argv[3];
  char* perm = argv[4];
  printf("%s, %s, %s, %s", fileName, content, argv[3], argv[4]);


  char* end;
  long mode = strtol(perm, &end, 8);
  /* printf("%d", mode); */
  int fd = open(fileName, O_CREAT | O_RDWR, mode);

  int position = strtol(poz, &end, 10);
  printf("Content je velik %d", strlen(content));
  printf("%d", position);
  lseek(fd, position, SEEK_SET);
  write(fd, content, strlen(content));


  return 0;
}
