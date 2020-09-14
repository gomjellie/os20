#include "ttop.h"

/**
 * 디렉토리 이름이 숫자인지 확인 (PID 식별)
 */
bool is_number(char input[]) {
     size_t input_len = strlen(input);

     for (int i = 0; i < input_len; i++) {
          if (isdigit(input[i]) == 0) // 숫자가 아니면 0 리턴함
               return false;
     }

     return true;
}

/**
 * CPU 사용 퍼센트 기준 정렬
 */
void sort_proc(stat_t stats[], int n) {
     stat_t aux;
     int i = 0;
     int j;

     while (i < n) {
          j = 0;
          while (j < i) {
               if (stats[j].cpu_usage < stats[i].cpu_usage) {
                    aux = stats[j];
                    stats[j] = stats[i];
                    stats[i] = aux;
               }
               j = j + 1;
          }
          i = i + 1;
     }
}

/**
 * 진입 함수
 */
void print_all_proc(stat_t stats[], int n) {
     int hour, minute, second;

     for (int i = 1; i < 70; i++) {
          mvprintw(0, i, "=");
     }

     mvprintw(1, 1, "     PID |");
     mvprintw(1, 11, "  PR |");
     mvprintw(1, 17, "   S |");
     mvprintw(1, 27, "    %%CPU |");
     mvprintw(1, 42, "    TIME |");
     mvprintw(1, 53, "COMMAND");

     for (int i = 1; i < 70; i++) {
          mvprintw(2, i, "=");
     }

     for (int i = 0; i < n; i++) {
          hour = stats[i].time / 3600;
          minute = (stats[i].time - (3600 * hour)) / 60;
          second = (stats[i].time - (3600 * hour) - (minute * 60));

          mvprintw(i + 3, 1, "%8d |\t", stats[i].pid);
          mvprintw(i + 3, 11, "%4ld |\t", stats[i].priority);
          mvprintw(i + 3, 17, "%4c |\t", stats[i].state);
          mvprintw(i + 3, 27, "%6.2f %% |\t", stats[i].cpu_usage);
          mvprintw(i + 3, 42, "%02d:%02d:%02d |\t", hour, minute, second);
          mvprintw(i + 3, 53, "%s\t\n", stats[i].command);
     }
}

/**
 * 프로세스 실행 시간 계산
 */
double get_seconds(unsigned long long starttime) {
     long int uptime;
     long int hertz;
     double seconds;

     FILE *uptimeFile = fopen("/proc/uptime", "r");

     if (!uptimeFile)
          return 0;


     fscanf(uptimeFile, "%ld", &uptime);
     fclose(uptimeFile);

     hertz = sysconf(_SC_CLK_TCK);

     if (hertz == 0)
          return 0;

     seconds = (double)(uptime - starttime / hertz);

     return seconds;
}

/*
 * CPU 사용 퍼센트 계산
 */
double get_cpu_usage(unsigned long int utime, unsigned long int stime, long int cutime, long int cstime, unsigned long long starttime) {
     double cpu_usage, seconds;
     long int total_time, hertz;

     hertz = sysconf(_SC_CLK_TCK);
     total_time = utime + stime;
     total_time = total_time + cstime + cutime;

     if (hertz == 0)
          return 0;

     seconds = get_seconds(starttime);

     if (seconds == 0)
          return 0;


     cpu_usage = 100 * ((total_time / hertz) / seconds);

     return cpu_usage;
}

/**
 * 프로세스 디렉토리를 받아서 stat 파일을 읽음.
 */
void read_stat(char *path, int position, stat_t stats[]) {
     char *pth = malloc(sizeof(char) * 90);

     unsigned long int utime, stime;
     long int cutime, cstime;
     unsigned long long starttime;

     strcpy(pth, path);
     strcat(pth, "stat");

     FILE *file = fopen(pth, "r");

     if (!file) {
          return;
     }

     // 0   1    2 3 4 5 6 7 8 9 0 1 2 3  4 5 6  7 8 9 0 1      22       23                   24 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     // 1 (init) S 0 1 1 0 0 0 0 0 0 0 0 12 0 0 20 0 2 0 0 950688743424 103 18446744073709551615 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
     fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld",
          &stats[position].pid,         //  0st , pid          %d  ,
          stats[position].command,      //  1st , comm         %s  , (zsh)
          &stats[position].state,       //  2nd , state        %c  , one of (S, R, Z, T, ...) 
          &stats[position].ppid,        //  3rd , ppid         %d  ,
          &stats[position].pgrp,        //  4th , pgrp         %d  , process group ID of the process.
          &stats[position].session,     //  5th , session      %d  ,
          &stats[position].tty_nr,      //  6th , tty_nr       %d  ,
          &stats[position].tpgid,       //  7th , tpgid        %d  ,
          &stats[position].flags,       //  8th , flags        %u  ,
          &stats[position].mintflt,     //  9th , mintflt      %lu ,
          &stats[position].cminflt,     // 10th , cminflt      %lu ,
          &stats[position].majflt,      // 11th , majflt       %lu ,
          &stats[position].cmajflt,     // 12th , cmajflt      %lu ,
          &utime,                       // 13th , utime        %lu , 
          &stime,                       // 14th , stime        %lu , 
          &cutime,                      // 15th , cutime       %ld ,
          &cstime,                      // 16th , cstime       %ld , utime + stime + cstime => total_time
          &stats[position].priority,    // 17th , priority     %ld , mostly value of 20
          &stats[position].nice,        // 18th , nice         %ld , 
          &stats[position].num_threads, // 19th , num_threads  %ld , 
          &stats[position].iteralvalue, // 20th , iteralvalue  %ld ,
          &starttime,                   // 21th , starttime    %llu,
          &stats[position].vsize,       // 22th , vsize        %lu ,
          &stats[position].rss);        // 23th , rss          %ld ,
     
     fclose(file);

     stats[position].cpu_usage = get_cpu_usage(utime, stime, cutime, cstime, starttime);
     stats[position].time = get_seconds(starttime);

     free(pth);
}

/**
 * /proc 디렉토리에서 검색
 * 이름의 숫자로 디렉토리를 확인합니다.
 * 찾은 디렉토리를 전달하는 read_print_stat 함수를 호출합니다.
 */
void read_all_proc(int row, int col) {
     int i = 0;
     DIR *directory;
     struct dirent *dir;
     stat_t stats[300];
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

          read_stat(current_path, i, stats);
          i ++;
     }

     sort_proc(stats, i - 1);
     print_all_proc(stats, 15);

     free(current_path);
}
