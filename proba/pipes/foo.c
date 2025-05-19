#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
   pid_t childpid;
   int fd[2];


   if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
      perror("Neuspjesno kreiranje pipe-a");
      return 1;
   }

   if (childpid == 0) {                                  /* ls komanda predstavlja child */
      if (dup2(fd[1], STDOUT_FILENO) == -1)
         perror("Neuspjesna redirekcija standardnog izlaza ls komande");
      else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
         perror("Neuspjesno zatvaranje file descriptor-a na komandi ls");
      else {
         execlp("ls", "ls", "-l", NULL);
         perror("Neuspjesno izvrsenje ls komande");
      }
      return 1;
   }
   if (dup2(fd[0], STDIN_FILENO) == -1)               /* wc komanda predstavlja parent */
   perror("Neuspjesna redirekcija standardnog ulaza wc komande");
   else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
       perror("Neuspjesno zatvaranje file descriptor-a na komandi wc");
   else {
      execlp("wc", "wc", "-l", NULL);
      perror("Neuspjesno izvrsenje wc komande");
   }
   return 1;
}


