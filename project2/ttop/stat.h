#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#ifndef __STAT_H__
#define __STAT_H__

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
     double mem_usage;
     double time;
     size_t shared;
} stat_t;

typedef struct _state {
     int running;
     int sleeping;
     int stopped;
     int zombie;
} state_count_t;

void stat_update(stat_t *this, int pid);

int stat_cmp(const void *stat1, const void *stat2);

void stats_update(stat_t stats[], int *stats_length);

void stats_count_state(const stat_t stats[], int stats_length, state_count_t *res);

#endif /* __STAT_H__ */
