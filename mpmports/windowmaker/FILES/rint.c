#include <math.h>

double rint(double x) {
    double l = floor(x);
    return fmod(x,l) >= (double) 0.5 ? ceil(x) : l;
}

