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
  int j;
  int fd[2];
  int is_pipe = -1;
  char * buffer = NULL;
  char * new_argv1[MAXARG];
  char * new_argv2[MAXARG];
  char * token;
  const char s[] = " \t\r\n\v\f";
  size_t maxarg = MAXARG;
  ssize_t nbytes;
  pid_t pid1;
  pid_t pid2;
  pid_t pid3;

  while (1) {
    printf("> ");

    if ((nbytes = getline(&buffer, &maxarg, stdin)) == -1) {
      return 1;  
    }

    i = 0;
    j = 0;
    token = strtok(buffer, s);

    while (token != NULL) {

      if (*token == '|') {
        is_pipe = 1;
      } else if (is_pipe == 1) {
        new_argv2[j++] = token;
      } else {
        new_argv1[i++] = token;
      }

      token = strtok(NULL, s);

    }

    new_argv1[i] = (char *) 0;
    new_argv2[j] = (char *) 0;

    if (is_pipe) {
      
      pipe(fd);
      pid2 = fork();
      if (pid2 == 0) {

        dup2(fd[1], STDOUT_FILENO);

        close(fd[0]);
        close(fd[1]);

        execvp(new_argv1[0], new_argv1);

        perror("execvp");
        exit(EXIT_FAILURE);

      } else {

        pid3 = fork();

        if (pid3 == 0) {

          dup2(fd[0], STDIN_FILENO);

          close(fd[0]);
          close(fd[1]);

          execvp(new_argv2[0], new_argv2);

          perror("execvp");
          exit(EXIT_FAILURE);

        }

        close(fd[0]);
        close(fd[1]);
        waitpid(pid2, 0, 0);
        waitpid(pid3, 0, 0);    
      }
    } else {

      pid1 = fork();

      if (pid1 > 0) {
        waitpid(pid1, 0, 0);
      } else {
        execvp(new_argv1[0], new_argv1);

        perror("execvp");
        exit(EXIT_FAILURE);
      }
    }
  }

  return 0;
} 

