#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_NUM_PIPES  64

char **tokenize(char *line);

typedef struct queue {
    int front;
    int rear;
    int body[128];
} queue_t;

queue_t *queue_new();
void queue_push(queue_t *this, int elem);
int queue_front(queue_t *this);
bool queue_pop(queue_t *this);
bool queue_empty(queue_t *this);
void queue_del(queue_t *this);
int queue_length(queue_t *this);

int main(int argc, char* argv[]) {
    char line[MAX_INPUT_SIZE];
    char **tokens; size_t token_len;

    FILE *fp;
    int pipefds[MAX_NUM_PIPES][2];
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
        for (int i = 0; tokens[i] != NULL; i++) token_len = i;

        queue_t *pipe_queue = queue_new(); // |가 tokens중에서 몇번째 인덱스에 있는지 스택에 저장
        
        if (tokens[0] != NULL) queue_push(pipe_queue, -1); // 빈 라인의 입력은 거른다.
        for (int pipe_idx = 0; tokens[pipe_idx] != NULL; pipe_idx++) { 
            // |를 찾아서 NULL로 바꾸고 pipe_queue에 tokens 에서 |의 인덱스를 push
            if (tokens[pipe_idx][0] != '|') continue; 
            queue_push(pipe_queue, pipe_idx);
            free(tokens[pipe_idx]);
            tokens[pipe_idx] = NULL;
        }
        int cmd_length = queue_length(pipe_queue); // ls | grep .c | wc 같은경우 cmd 3개니까 cmd_length = 3

        for (int i = 0; i < cmd_length; i++) {
            pipe(pipefds[i]);
        }
        
        int cmdc = 0; // cmd counter
        while (!queue_empty(pipe_queue)) {
            cmdc++;
            pid = fork();

            if (pid != 0) {
                queue_pop(pipe_queue);
                continue;
            }

            if (cmdc) // not the first command
                if (dup2(pipefds[cmdc - 1][0], 0) < 0) fprintf(stderr, "line 124: dup2() err");
            if (cmdc != cmd_length) // if not the last command
                if (dup2(pipefds[cmdc][1], 1) < 0) fprintf(stderr, "line 127: dup2() err");

            for (int i = 0; i < cmdc; i++) { close(pipefds[i][0]); close(pipefds[i][1]); }

            int div_index = queue_front(pipe_queue); // |의 인덱스
            int res = execvp(tokens[div_index + 1], tokens + 1 + div_index);
            if (res == -1) {
                fprintf(stderr, "SSUShell : Incorrect command\n");
                exit(1);
            }
            queue_pop(pipe_queue);
        }

        for (int i = 0; i < cmd_length; i++) {
            close(pipefds[i][0]); close(pipefds[i][1]);
        }

        for (int i = 0; i < cmd_length; i++) {
            wait(&status);
        }

        queue_del(pipe_queue);
        for (int i = 0; i < token_len; i++) free(tokens[i]);
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

queue_t *queue_new() {
    queue_t *this = malloc(sizeof(queue_t));
    this->front = 0;
    this->rear = 0;
    return this;
}

void queue_push(queue_t *this, int elem) {
    this->body[this->rear++] = elem;
}

int queue_front(queue_t *this) {
    return this->body[this->front];
}

bool queue_pop(queue_t *this) {
    if (this->front == this->rear) return false;
    this->body[this->front++] = 0;
    return true;
}

bool queue_empty(queue_t *this) {
    return this->rear == this->front;
}

void queue_del(queue_t *this) {
    free(this);
}

int queue_length(queue_t *this) {
    return this->rear - this->front;
}
