#include "ttop.h"

/**
 * 디렉토리 이름이 숫자인지 확인 (PID 식별)
 */
bool is_number(char input[]) {
     size_t input_len = strlen(input);

     for (int i = 0; i < input_len; i++) {
          if (isdigit(input[i]) != 0) return false; // isdigit 은 0~9가 있으면 0을 리턴함
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

     if (!uptimeFile) {
          return 0;
     }

     fscanf(uptimeFile, "%ld", &uptime);
     fclose(uptimeFile);

     hertz = sysconf(_SC_CLK_TCK);

     if (hertz == 0) {
          return 0;
     }

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

     if (hertz == 0) {
          return 0;
     }

     seconds = get_seconds(starttime);

     if (seconds == 0) {
          return 0;
     }

     cpu_usage = 100 * ((total_time / hertz) / seconds);

     return cpu_usage;
}

/**
 * 프로세스 디렉토리를 받아서 stat 파일을 읽음.
 */
void read_stat(char *path, int position, stat_t stats[]) {
     char *pth = malloc(sizeof(char) * 90);

     int null_int;
     unsigned int null_uns_int;
     unsigned long int utime, stime, null_uns_long_int;
     long int cutime, cstime, null_long_int;
     unsigned long long starttime;

     strcpy(pth, path);
     strcat(pth, "stat");

     FILE *file = fopen(pth, "r");

     if (!file) {
          return;
     }

     fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu",
          &stats[position].pid,
          stats[position].command,
          &stats[position].state,
          &null_int,
          &null_int,
          &null_int,
          &null_int,
          &null_uns_int,
          &null_int,
          &null_uns_long_int,
          &null_uns_long_int,
          &null_uns_long_int,
          &null_uns_long_int,
          &utime,
          &stime,
          &cutime,
          &cstime,
          &stats[position].priority,
          &null_long_int,
          &null_long_int,
          &null_long_int,
          &starttime);

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

          if (is_number(directory_name_buffer) == true) { // ??!
               continue;
          }

          strcpy(current_path, "/proc/");
          strcat(current_path, directory_name_buffer);
          strcat(current_path, "/");

          read_stat(current_path, i, stats);
          i = i + 1;
     }

     sort_proc(stats, i - 1);
     print_all_proc(stats, 15);

     free(current_path);
}
