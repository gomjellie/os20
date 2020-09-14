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

#define BUFFER_LENGTH 32

typedef struct stat {
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
} stat_t;

/* /proc/<pid>/stat 파일을 읽어서 데이터 갱신 */
void stat_update(stat_t *this, char *pid);

/**
 * CPU 사용 퍼센트 기준 내림차순 정렬
 * 퍼센트가 같을경우 pid기준 오름차순 정렬
 */
int stat_cmp(const void *stat1, const void *stat2);

/**
 * /proc 디렉토리에서 숫자이름(pid)들을 찾아서 그 프로세스들의 stat을 업데이트
 * stat_length 로 읽어온 pid개수를 저장해서 호출자에게 넘겨줌.
 */
void stats_update(stat_t stats[], int *stats_length);

#endif /* __TTOP_H__ */
