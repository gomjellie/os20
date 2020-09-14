#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifndef __STATM_H__
#define __STATM_H__

typedef struct statm {
    /**
       /proc/[pid]/statm
              Provides information about memory usage, measured in pages.
              The columns are:

                  size       (1) total program size
                             (same as VmSize in /proc/[pid]/status)
                  resident   (2) resident set size
                             (same as VmRSS in /proc/[pid]/status)
                  shared     (3) number of resident shared pages (i.e., backed by a file)
                             (same as RssFile+RssShmem in /proc/[pid]/status)
                  text       (4) text (code)
                  lib        (5) library (unused since Linux 2.6; always 0)
                  data       (6) data + stack
                  dt         (7) dirty pages (unused since Linux 2.6; always 0)
     */
    size_t size; // total program size (same as VmSize in /proc/[pid]/status)
    size_t resident;
    size_t shared; // number of resident shared pages (same as RssFile + RssShmem in /proc/[pid]/status)
    size_t text;   // text (code)
    size_t lib;
    size_t data;
    size_t dt;
} statm_t;

void statm_update(statm_t *this, int pid);

#endif /* __STATM_H__ */
