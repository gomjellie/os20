#include "stat.h"
#include "statm.h"

#include <sys/sysinfo.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static double __get_seconds(unsigned long long starttime);
static void __stat_update_cpu_usage(stat_t *this);
static void __stat_update_mem_usage(stat_t *this);

/**
 * CPU 사용 퍼센트 기준 내림차순 정렬
 * 퍼센트가 같을경우 MEM 기준 내림차순
 * 퍼센트 같을경우 pid기준 오름차순 정렬
 */
int stat_cmp(const void *stat1, const void *stat2) {
    const stat_t *s1 = stat1;
    const stat_t *s2 = stat2;
    
    return strcmp(s1->tty, s2->tty);
}

/* /proc/<pid>/stat 파일을 읽어서 데이터 갱신 */
void stat_update(stat_t *this, int pid) {
    char *path = malloc(32);

    sprintf(path, "/proc/%d/fd/0", pid);
    int fd = open(path, O_RDONLY);
    char *tty = ttyname(fd);
    if (tty != NULL)
        sprintf(this->tty, "%s", ttyname(fd));
    else
        this->tty[0] = '\0';

    close(fd);

    sprintf(path, "/proc/%d/stat", pid);

    FILE *file = fopen(path, "r");

    if (!file)
        return;

    fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld",
        &this->pid,        //  0st , pid        %d  ,
        this->command,     //  1st , comm        %s  , (zsh)
        &this->state,      //  2nd , state       %c  , one of (S, R, Z, T, ...) 
        &this->ppid,       //  3rd , ppid        %d  ,
        &this->pgrp,       //  4th , pgrp        %d  , process group ID of the process.
        &this->session,    //  5th , session     %d  ,
        &this->tty_nr,     //  6th , tty_nr      %d  ,
        &this->tpgid,      //  7th , tpgid       %d  ,
        &this->flags,      //  8th , flags       %u  ,
        &this->mintflt,    //  9th , mintflt     %lu ,
        &this->cminflt,    // 10th , cminflt     %lu ,
        &this->majflt,     // 11th , majflt      %lu ,
        &this->cmajflt,    // 12th , cmajflt     %lu ,
        &this->utime,      // 13th , utime       %lu , 
        &this->stime,      // 14th , stime       %lu , 
        &this->cutime,     // 15th , cutime      %ld ,
        &this->cstime,     // 16th , cstime      %ld , utime + stime + cstime => total_time
        &this->priority,    // 17th , priority    %ld , mostly value of 20
        &this->nice,       // 18th , nice        %ld , 
        &this->num_threads, // 19th , num_threads  %ld , 
        &this->iteralvalue, // 20th , iteralvalue  %ld ,
        &this->starttime,   // 21th , starttime    %llu,
        &this->vsize,      // 22th , vsize       %lu ,
        &this->rss);       // 23th , rss        %ld ,

    sscanf(this->command, "(%[^)])", this->command);
    fclose(file);

    __stat_update_cpu_usage(this);
    __stat_update_mem_usage(this);
    this->time = __get_seconds(this->starttime);

    free(path);
}

/* 프로세스 실행 시간 계산 */
static double __get_seconds(unsigned long long starttime) {
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
static void __stat_update_cpu_usage(stat_t *this) {
    long int total_time, clock_ticks;
    double process_seconds = __get_seconds(this->starttime);

    clock_ticks = sysconf(_SC_CLK_TCK);
    total_time = this->utime + this->stime + this->cutime;

    if (clock_ticks == 0) {
        this->cpu_usage = 0.0;
        return; // 0으로 나눌 수 없음
    }

    this->cpu_usage = 100.0 * ((total_time / clock_ticks) / process_seconds);
}

static void __stat_update_mem_usage(stat_t *this) {
    unsigned long total_mem; // TODO:
    statm_t statm;
    statm_update(&statm, this->pid);

    struct sysinfo info;
    sysinfo(&info);
    // system_totalram     = (uint64_t)(info.totalram * info.mem_unit);
    // system_availableram = (uint64_t)((info.freeram + info.bufferram) * info.mem_unit);
    // system_freeram      = (uint64_t)(info.freeram * info.mem_unit);
    total_mem = (info.totalram * info.mem_unit);

    this->shared = statm.shared;
    this->mem_usage = ((double)(statm.size)) * 100UL * 100UL / total_mem; // top 명령어랑 똑같은 값을 못구하겠음
}
