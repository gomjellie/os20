#include "stat.h"

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

/* /proc/<pid>/stat 파일을 읽어서 데이터 갱신 */
void stat_update(stat_t *this, char *pid) {
     char *pth = malloc(32);

     sprintf(pth, "/proc/%s/stat", pid);

     FILE *file = fopen(pth, "r");

     if (!file)
          return;

     // 0   1    2 3 4 5 6 7 8 9 0 1 2 3  4 5 6  7 8 9 0 1      22       23                   24 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     // 1 (init) S 0 1 1 0 0 0 0 0 0 0 0 12 0 0 20 0 2 0 0 950688743424 103 18446744073709551615 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
     fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld",
          &this->pid,         //  0st , pid          %d  ,
          this->command,      //  1st , comm         %s  , (zsh)
          &this->state,       //  2nd , state        %c  , one of (S, R, Z, T, ...) 
          &this->ppid,        //  3rd , ppid         %d  ,
          &this->pgrp,        //  4th , pgrp         %d  , process group ID of the process.
          &this->session,     //  5th , session      %d  ,
          &this->tty_nr,      //  6th , tty_nr       %d  ,
          &this->tpgid,       //  7th , tpgid        %d  ,
          &this->flags,       //  8th , flags        %u  ,
          &this->mintflt,     //  9th , mintflt      %lu ,
          &this->cminflt,     // 10th , cminflt      %lu ,
          &this->majflt,      // 11th , majflt       %lu ,
          &this->cmajflt,     // 12th , cmajflt      %lu ,
          &this->utime,       // 13th , utime        %lu , 
          &this->stime,       // 14th , stime        %lu , 
          &this->cutime,      // 15th , cutime       %ld ,
          &this->cstime,      // 16th , cstime       %ld , utime + stime + cstime => total_time
          &this->priority,    // 17th , priority     %ld , mostly value of 20
          &this->nice,        // 18th , nice         %ld , 
          &this->num_threads, // 19th , num_threads  %ld , 
          &this->iteralvalue, // 20th , iteralvalue  %ld ,
          &this->starttime,   // 21th , starttime    %llu,
          &this->vsize,       // 22th , vsize        %lu ,
          &this->rss);        // 23th , rss          %ld ,

     fclose(file);

     this->cpu_usage = stat_calc_cpu_usage(this);
     this->time = get_seconds(this->starttime);

     free(pth);
}

/**
 * /proc 디렉토리에서 숫자이름(pid)들을 찾아서 그 프로세스들의 stat을 업데이트
 * stat_length 로 읽어온 pid개수를 저장해서 호출자에게 넘겨줌.
 */
void stats_update(stat_t stats[], int *stats_length) {
     int i = 0;
     DIR *directory;
     struct dirent *dir;
     char *directory_name_buffer;

     directory = opendir("/proc");

     if (directory == NULL) return;

     while ((dir = readdir(directory)) != NULL && i < 300) {
          directory_name_buffer = dir->d_name;

          if (is_number(directory_name_buffer) == false)
               continue;

          stat_update(&stats[i], directory_name_buffer);
          i ++;
     }

     *stats_length = i;
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
