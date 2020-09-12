#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line);

int main(int argc, char* argv[]) {
    char  line[MAX_INPUT_SIZE];
    char  **tokens;
    int i;

    FILE *fp;
    pid_t pid;
    int status;
    if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp < 0) {
            printf("File doesn't exists.");
            return -1;
        }
    }

    while (1) {
        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        if (argc == 2) { // batch mode
            if (fgets(line, sizeof(line), fp) == NULL) { // file reading finished
                break;
            }
            line[strlen(line) - 1] = '\0';
        } else { // interactive mode
            printf("$ ");
            scanf("%[^\n]", line);
            getchar();
        }
        // printf("Command entered: %s (remove this debug output later)\n", line);
        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
        //do whatever you want with the commands, here we just print them

        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork error");
        } else if (pid == 0) { // child
            int res = execvp(tokens[0], tokens);
            if (res == -1) {
                fprintf(stderr, "SSUShell : Incorrect command\n");
                exit(1);
            }
            exit(0);
        }

        pid = wait(&status); // wait for child to die

        if (pid < 0) {
            fprintf(stderr, "자식 프로세스가 제대로 종료되지 않음 status(%d)\n", status);
        } else if (WIFSIGNALED(status)) { // 자식 프로세스 비정상 종료
            fprintf(stderr, "✘");
        }

        // Freeing the allocated memory
        for(i = 0; tokens[i] != NULL; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }
    return 0;
}

/**
 * Splits the string by space and returns the array of tokens
 */
char **tokenize(char *line) {
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i = 0; i < strlen(line); i++) {
        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0) {
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE * sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        } else {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}
