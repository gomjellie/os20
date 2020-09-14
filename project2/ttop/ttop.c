#include "ttop.h"

/**
 * private functions
 */
static double get_seconds(unsigned long long starttime);
static bool is_number(char input[]);

/**
 * CPU 사용 퍼센트 기준 정렬
 */
int stat_cmp(const void *p1, const void *p2) {
     const stat_t *s1 = p1;
     const stat_t *s2 = p2;
     
     if (s1->cpu_usage < s2->cpu_usage)
          return 1;
     if (s1->cpu_usage == s2->cpu_usage) {
          if (s1->pid > s2->pid)
               return 1;
          if (s1->pid == s2->pid)
               return 0;
          return -1;
     }
     return -1;
}

/* CPU 사용 퍼센트 계산 */
double stat_calc_cpu_usage(stat_t *this) {
     long int total_time, clock_ticks;
     double process_seconds = get_seconds(this->starttime);

     clock_ticks = sysconf(_SC_CLK_TCK);
     total_time = this->utime + this->stime + this->cutime;

     if (clock_ticks == 0)
          return 0.0; // 0으로 나눌 수 없음

     return 100.0 * ((total_time / clock_ticks) / process_seconds);
}

/* stat 파일을 읽어서 stats[idx]를 갱신함. */
void stat_update(char *path, int idx, stat_t stats[]) {
     char *pth = malloc(sizeof(char) * 90);

     strcpy(pth, path);
     strcat(pth, "stat");

     FILE *file = fopen(pth, "r");

     if (!file) {
          return;
     }

     // 0   1    2 3 4 5 6 7 8 9 0 1 2 3  4 5 6  7 8 9 0 1      22       23                   24 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     // 1 (init) S 0 1 1 0 0 0 0 0 0 0 0 12 0 0 20 0 2 0 0 950688743424 103 18446744073709551615 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
     fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld",
          &stats[idx].pid,         //  0st , pid          %d  ,
          stats[idx].command,      //  1st , comm         %s  , (zsh)
          &stats[idx].state,       //  2nd , state        %c  , one of (S, R, Z, T, ...) 
          &stats[idx].ppid,        //  3rd , ppid         %d  ,
          &stats[idx].pgrp,        //  4th , pgrp         %d  , process group ID of the process.
          &stats[idx].session,     //  5th , session      %d  ,
          &stats[idx].tty_nr,      //  6th , tty_nr       %d  ,
          &stats[idx].tpgid,       //  7th , tpgid        %d  ,
          &stats[idx].flags,       //  8th , flags        %u  ,
          &stats[idx].mintflt,     //  9th , mintflt      %lu ,
          &stats[idx].cminflt,     // 10th , cminflt      %lu ,
          &stats[idx].majflt,      // 11th , majflt       %lu ,
          &stats[idx].cmajflt,     // 12th , cmajflt      %lu ,
          &stats[idx].utime,       // 13th , utime        %lu , 
          &stats[idx].stime,       // 14th , stime        %lu , 
          &stats[idx].cutime,      // 15th , cutime       %ld ,
          &stats[idx].cstime,      // 16th , cstime       %ld , utime + stime + cstime => total_time
          &stats[idx].priority,    // 17th , priority     %ld , mostly value of 20
          &stats[idx].nice,        // 18th , nice         %ld , 
          &stats[idx].num_threads, // 19th , num_threads  %ld , 
          &stats[idx].iteralvalue, // 20th , iteralvalue  %ld ,
          &stats[idx].starttime,   // 21th , starttime    %llu,
          &stats[idx].vsize,       // 22th , vsize        %lu ,
          &stats[idx].rss);        // 23th , rss          %ld ,

     fclose(file);

     stats[idx].cpu_usage = stat_calc_cpu_usage(&stats[idx]);
     stats[idx].time = get_seconds(stats[idx].starttime);

     free(pth);
}

/**
 * /proc 디렉토리에서 검색
 * 이름의 숫자로 디렉토리를 확인합니다.
 */
void stat_parse(stat_t stats[], int *stat_length) {
     int i = 0;
     DIR *directory;
     struct dirent *dir;
     char *directory_name_buffer, *current_path;

     directory = opendir("/proc");
     current_path = malloc(sizeof(char) * 10);

     if (directory == NULL) return;

     while ((dir = readdir(directory)) != NULL && i < 300) {
          directory_name_buffer = dir->d_name;

          if (is_number(directory_name_buffer) == false)
               continue;

          strcpy(current_path, "/proc/");
          strcat(current_path, directory_name_buffer);
          strcat(current_path, "/");

          stat_update(current_path, i, stats);
          i ++;
     }

     *stat_length = i;
     free(current_path);
}

/* 디렉토리 이름이 숫자인지 확인 (PID 식별) */
static bool is_number(char input[]) {
     size_t input_len = strlen(input);

     for (int i = 0; i < input_len; i++) {
          if (isdigit(input[i]) == 0) // 숫자가 아니면 0 리턴함
               return false;
     }

     return true;
}

/* 프로세스 실행 시간 계산 */
static double get_seconds(unsigned long long starttime) {
     long int uptime;
     long int clock_ticks;
     double seconds;

     FILE *uptimeFile = fopen("/proc/uptime", "r");

     if (!uptimeFile)
          return 0;


     fscanf(uptimeFile, "%ld", &uptime);
     fclose(uptimeFile);

     clock_ticks = sysconf(_SC_CLK_TCK);

     if (clock_ticks == 0)
          return 0;

     seconds = (double)(uptime - (starttime / clock_ticks));

     return seconds;
}
