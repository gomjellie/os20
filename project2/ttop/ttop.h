#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <ncurses.h>
#include <time.h>

#ifndef __TTOP_H__
#define __TTOP_H__

#define BUFFER_LENGTH 30

struct stat {
     int pid;
     long int priority;
     char state;
     double cpu_usage;
     double time;
     char command[BUFFER_LENGTH];
} typedef stat_t;

/**
 * 프로세스 실행 시간 계산
 */
double get_seconds(unsigned long long starttime);

/**
 * CPU 사용 퍼센트 기준 정렬
 */
void sort_proc(stat_t stats[], int n);

/**
 * CPU 사용 퍼센트 계산
 */
double get_cpu_usage(unsigned long int utime, unsigned long int stime, long int cutime, long int cstime, unsigned long long starttime);

/**
 * 디렉토리 이름이 숫자인지 확인 (PID 식별)
 */
bool is_number(char input[]);

/**
 * 프로세스 디렉토리를 받아서 stat 파일을 읽음.
 */
void read_stat(char * path, int position, stat_t stats[]);

/**
 * /proc 디렉토리에서 검색
 * 이름의 숫자로 디렉토리를 확인합니다.
 * 찾은 디렉토리를 전달하는 read_print_stat 함수를 호출합니다.
 */
void read_all_proc(int row, int col);

#endif /* __TTOP_H__ */
