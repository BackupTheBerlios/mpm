#include <sys/resource.h>

int nice(int inc) {
    int new = getpriority(PRIO_PROCESS, 0) + inc;
    return setpriority(PRIO_PROCESS, 0, new) ? -1 : new;
}
