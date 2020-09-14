#include "stat.h"

static double get_seconds(unsigned long long starttime);
static bool is_number(char input[]);
static void stat_update_cpu_usage(stat_t *this);

/**
 * CPU 사용 퍼센트 기준 내림차순 정렬
 * 퍼센트가 같을경우 pid기준 오름차순 정렬
 */
int stat_cmp(const void *stat1, const void *stat2) {
     const stat_t *s1 = stat1;
     const stat_t *s2 = stat2;
     
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

/* /proc/<pid>/stat 파일을 읽어서 데이터 갱신 */
void stat_update(stat_t *this, char *pid) {
     char *pth = malloc(32);

     sprintf(pth, "/proc/%s/stat", pid);

     FILE *file = fopen(pth, "r");

     if (!file)
          return;

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

     stat_update_cpu_usage(this);
     this->time = get_seconds(this->starttime);

     free(pth);
}

/**
 * /proc 디렉토리에서 숫자이름(pid)들을 찾아서 그 프로세스들의 stat을 업데이트
 * stat_length 로 읽어온 pid개수를 저장해서 호출자에게 넘겨줌.
 */
void stats_update(stat_t stats[], int *stats_length) {
     int len = 0;
     DIR *directory;
     struct dirent *dir;
     char *pid_buffer;

     directory = opendir("/proc");

     if (directory == NULL) return;

     while ((dir = readdir(directory)) != NULL && len < 256) {
          pid_buffer = dir->d_name;

          if (is_number(pid_buffer) == false)
               continue;

          stat_update(&stats[len], pid_buffer);
          len ++;
     }

     *stats_length = len;
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

/* CPU 사용 퍼센트 계산후 업데이트 */
static void stat_update_cpu_usage(stat_t *this) {
     long int total_time, clock_ticks;
     double process_seconds = get_seconds(this->starttime);

     clock_ticks = sysconf(_SC_CLK_TCK);
     total_time = this->utime + this->stime + this->cutime;

     if (clock_ticks == 0) {
          this->cpu_usage = 0.0;
          return; // 0으로 나눌 수 없음
     }

     this->cpu_usage = 100.0 * ((total_time / clock_ticks) / process_seconds);
}
