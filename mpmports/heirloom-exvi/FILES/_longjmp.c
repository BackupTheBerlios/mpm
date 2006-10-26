#include <setjmp.h>

void _longjmp(jmp_buf env, int val) {
    longjmp(env, val);
}
