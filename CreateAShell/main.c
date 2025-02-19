#include <Windows.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFSIZE 64
#define DELIM " \t\r\n\a"
#define CMD_SIZE 1024

int cd(char **args);
int help(char **args);
int exit_sh(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
  };
  
  int (*builtin_func[]) (char **) = {
    &cd,
    &help,
    &exit_sh
  };
  
  int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
  }
  
  /*
    Builtin function implementations.
  */
  int cd(char **args)
  {
    if (args[1] == NULL) {
      fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
      if (chdir(args[1]) != 0) {
        perror("lsh");
      }
    }
    return 1;
  }
  
  int help(char **args)
  {
    int i;
    printf("Testing\n");
  
    for (i = 0; i < num_builtins(); i++) {
      printf("  %s\n", builtin_str[i]);
    }
  
    printf("Use the man command for information on other programs.\n");
    return 1;
  }
  
  int exit_sh(char **args)
  {
    return 0;
  }


int launch(char **args){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    char cmd[CMD_SIZE] = {0};\
    int i;
    for(i = 0; args[i] != NULL; i++){
        strcat(cmd, args[i]);
        strcat(cmd, " ");
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if(!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
        fprintf(stderr, "Error creating process\n");
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 1;
}

int execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}


char **split_line(char *line){
    int buffer_size = BUFFSIZE;
    int index = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIM);
    while(token != NULL){
        tokens[index] = token;
        index++;

        if(index >= buffer_size){
            buffer_size += BUFFSIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char*));

            if(!tokens){
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DELIM);
    }
    tokens[index] = NULL;
    return tokens;
}

char *read_line(){
    size_t buffer_size = CMD_SIZE;  // Set a fixed buffer size
    char *line = malloc(buffer_size * sizeof(char));

    if (!line) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    if (fgets(line, buffer_size, stdin) == NULL) {
        if (feof(stdin)) {
            printf("\nExiting shell...\n");
            exit(EXIT_SUCCESS);
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}


void run_shell(){
    char *line;
    char **args;
    int status = 1;

    while(status){
        printf(">> ");
        line = read_line();
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    }

}



int main(){

    run_shell();

    return EXIT_SUCCESS;
}