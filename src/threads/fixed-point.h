#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H

#include <stdint.h>

typedef int fixed_pt;
#define F (1 << 14) // 2^14

//Convertion to fixed point
fixed_pt convert_to_fixed(int n);

//Convertion to integer (rounding towards zero)
int convert_to_int_zero(fixed_pt x);

//Convertion to integer (rounding to nearest)
int convert_to_int_nearest(fixed_pt x);

//Addition of two fixed point numbers
fixed_pt add_fixed(fixed_pt x, fixed_pt y);

//Addition of fixed point and integer
fixed_pt add_fixed_int(fixed_pt x, int n);

//Subtraction of two fixed point numbers
fixed_pt sub_fixed(fixed_pt x, fixed_pt y);

//Subtraction of fixed point and integer
fixed_pt sub_fixed_int(fixed_pt x, int n);

//Multiplication of two fixed point numbers
fixed_pt mul_fixed(fixed_pt x, fixed_pt y);

//Multiplication of fixed point and integer
fixed_pt mul_fixed_int(fixed_pt x, int n);

//Division of two fixed point numbers
fixed_pt div_fixed(fixed_pt x, fixed_pt y);

//Division of fixed point and integer
fixed_pt div_fixed_int(fixed_pt x, int n);

#endif /* THREADS_FIXED_POINT_H */
