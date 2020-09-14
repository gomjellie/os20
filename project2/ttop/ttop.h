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
     // /proc/PID/stat 에 있는 정보를 순서대로 나열함.
     int pid;
     char command[BUFFER_LENGTH];
     char state;
     int ppid;
     int pgrp;
     int session;
     int tty_nr;
     int tpgid;
     unsigned int flags;
     unsigned long int mintflt;
     unsigned long int cminflt;
     unsigned long int majflt;
     unsigned long int cmajflt;
     unsigned long int utime;
     unsigned long int stime;
     long int cutime;
     long int cstime;
     long int priority;
     long int nice;
     long int num_threads;
     long int iteralvalue;
     unsigned long long starttime;
     unsigned long int vsize;
     long int rss;

     // 아래 변수들은 /proc/PID/stat 에 있는 정보가 아님.
     // 별도의 계산을 통해서 구해야하는 값들.
     double cpu_usage;
     double time;
} typedef stat_t;

/**
 * CPU 사용 퍼센트 계산
 */
double stat_calc_cpu_usage(stat_t *this);

/**
 * 프로세스 실행 시간 계산
 */
double get_seconds(unsigned long long starttime);

/**
 * CPU 사용 퍼센트 기준 정렬
 */
void sort_proc(stat_t stats[], int n);

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
void stat_parse(stat_t stats[], int *stat_length);

void on_draw(stat_t stats[], int stats_len);

#endif /* __TTOP_H__ */
