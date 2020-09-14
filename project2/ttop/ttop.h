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

/* CPU 사용 퍼센트 계산 */
double stat_calc_cpu_usage(stat_t *this);

/**
 * CPU 사용 퍼센트 기준 내림차순 정렬
 * 퍼센트가 같을경우 pid기준 오름차순 정렬
 */
int stat_cmp(const void *p1, const void *p2);

/* stat 파일을 읽어서 stats[idx]를 갱신함. */
void stat_update(char * path, int idx, stat_t stats[]);

/**
 * /proc 디렉토리에서 검색
 * 이름의 숫자로 디렉토리를 확인합니다.
 */
void stat_parse(stat_t stats[], int *stat_length);

#endif /* __TTOP_H__ */
