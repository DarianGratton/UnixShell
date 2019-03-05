#define _GNU_SOURCE

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXARG 1024

int main(void) {
  int i;
  int ifd = 0;
  int ofd = 0;
  char * buffer = NULL;
  char * new_argv[MAXARG];
  char * token;
  const char s[] = " \t\r\n\v\f";
  size_t maxarg = MAXARG;
  ssize_t nbytes;
  pid_t pid;

  while (1) {
    printf("> ");

    if ((nbytes = getline(&buffer, &maxarg, stdin)) == -1) {
      return 1;  
    }

    if ((pid = fork()) == -1) {
      perror("fork");
      return 2;
    }

    if (pid > 0) {      /* Parent */
      wait(0);
    } else {            /* Child */
      i = 0;
      token = strtok(buffer, s);

      while(token != NULL) {

        if (*token == '<') {
          token++;
          if ((ofd = open(token, O_CREAT | O_WRONLY | O_TRUNC, 0660)) == -1) {
            perror("ofd open");
            return 5;    
          } 

          if (dup2(ofd, STDOUT_FILENO) == -1) {
            perror("dup2");
            return 6;
          }

          close(ofd);

        } else if (*token == '>') {
          
          token++;
          if ((ifd = open(token, O_RDONLY)) == -1) {
            perror("open");
            return 3;
          }

          if (dup2(ifd, STDIN_FILENO) == -1) {
            perror("dup2");
            return 4;
          }

          close(ifd);

        } else {
          new_argv[i] = token;
          i++;
        }

        token = strtok(NULL, s);

     }

      new_argv[i] = (char *) 0;
      execvp(new_argv[0], new_argv);

      perror("execvp");
      return 3;
    }
  }  

  return 0;
}
