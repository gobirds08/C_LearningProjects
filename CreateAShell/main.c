#include <stdlib.h>
#include <stdio.h>


char *read_line(){
    char* line = NULL;
    ssize_t buffer_size = 0;

    if(getline(&line, &buffer_size, stdin) == -1){
        if(feof(stdin)){
            exit(EXIT_SUCCESS);
        }else{
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