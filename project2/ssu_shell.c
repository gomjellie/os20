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
#define TOKEN_DEL(TOKENS) \
for (int i = 0; TOKENS[i] != NULL; i++) {\
    free(TOKENS[i]); \
} \
free(TOKENS); \

int main(int argc, char* argv[]) {
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i;

    FILE *fp;
    int pipefd[2];
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
        if (pid == 0) { // child
            int res = execvp(tokens[0], tokens);
            if (res == -1) {
                fprintf(stderr, "SSUShell : Incorrect command\n");
                TOKEN_DEL(tokens);
                exit(1);
            }
            TOKEN_DEL(tokens);
            return 0;
        } else {
            pid = wait(&status); // wait for child to die

            TOKEN_DEL(tokens);
        }
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
