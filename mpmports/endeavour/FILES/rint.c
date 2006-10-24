#include <limits.h>
double rint(double x) {
    if (x>=INT_MAX) return INT_MAX;
    if (x<=INT_MIN) return INT_MIN;
    return (double)((int)x);
}
