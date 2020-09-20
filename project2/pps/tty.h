
#ifndef __TTY_H__
#define __TTY_H__

typedef struct _tty { // /dev/tty에서 뽑아낼 정보중에 필요한 정보들 저장
    int major;
    int minor;
    char tid[16];
} tty_t;

tty_t *tty_new();
void tty_del(tty_t *this);
void tty_update(tty_t *this, char *tid);
int tty_is_same(tty_t *this, int ttynr);

#endif /* __TTY_H__ */
