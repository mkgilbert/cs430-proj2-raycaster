/* 3d math with vectors */
#ifndef VECTORMATH_H
#define VECTORMATH_H
#endif

#ifdef VECTORMATH_H

// way to represent vectors in memory?
// diff between float and double --> 32 vs 64 bits
// use doubles!
#include <stdio.h>
#include <stdlib.h>

typedef double V3[3]; // array of double values to represent 

// inline says don't compile to a function somewhere in the program, just do
// this op whenever you see it (gives performance savings)
// C may not inline this, it might put it into the library to use. static tells
// it to put the function in a local namespace
static inline void v3_add(V3 a, V3 b, V3 c) {
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
}

static inline void v3_sub(V3 a, V3 b, V3 c) {
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

static inline void v3_scale(V3 a, double s, V3 b) {
    b[0] = s * a[0]; 
    b[1] = s * a[1]; 
    b[2] = s * a[2]; 
}

static inline double v3_dot(V3 a, V3 b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static inline void v3_cross(V3 a, V3 b, V3 c) {
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

void print_v3(V3 data){
    printf("V3: (%lf, %lf, %lf)\n", data[0], data[1], data[2]);
}

void example() {
    double data1[] = {1.0, 2.0, 3.0};
    double data2[] = {4.0, 5.0, 6.0};
    double result[3];
    v3_cross(data1, data2, result);
    print_v3(result);
    double x = v3_dot(data1, data2);
    printf("dot: %lf\n", x);
}

#endif
