/* ------------------------------------------------------------------------- */

/*
 * uprescue by Ivo van Poorten, Oct. 2006
 *
 * same as: service up /bin/rescue -dev /dev/rescue -args 4096 -period 4HZ
 *
 * Public Domain
 *
 */

/* ------------------------------------------------------------------------- */

#include <unistd.h>
#include <minix/com.h>
#include <minix/type.h>
#include <minix/syslib.h>

/* ------------------------------------------------------------------------- */

int uprescue(void) {
    message m;
    int request;
    int s;

    request = RS_UP;

    m.RS_CMD_ADDR = "/bin/rescue 4096";
    m.RS_CMD_LEN = 16;
    m.RS_DEV_MAJOR = 9; /* /dev/rescue */
    m.RS_PERIOD = 4; /* 4HZ */

    if ((s=_taskcall(RS_PROC_NR, request, &m)) != 0) {
        write(2, "uprescue: failed to send to RS\n", 31);
        _exit(s);
    }
    return m.m_type;
}

/* ------------------------------------------------------------------------- */

#ifndef NO_MAIN
int main(int argc, char **argv) {
    if (argc != 1) {
        write(2,"usage: uprescue\n", 16);
        _exit(-1);
    }
    return uprescue();
}
#endif

/* ------------------------------------------------------------------------- */
