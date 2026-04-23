#include <cstdint>

typedef int fixed_pt;
#define f (1 << 14) // 2^14

//Convertion to fixed point
fixed_pt convert_to_fixed(int n){
    return n * f;
}

//Convertion to integer (rounding towards zero)
int convert_to_int_zero(fixed_pt x){
    return x / f;
}

//Convertion to integer (rounding to nearest)
int convert_to_int_nearest(fixed_pt x){
    if(x >= 0){
        return (x + f / 2) / f;
    } 
    else {
        return (x - f / 2) / f;
    }
}

//Addition of two fixed point numbers
fixed_pt add_fixed(fixed_pt x, fixed_pt y){
    return x + y;
}

//Addition of fixed point and integer
fixed_pt add_fixed_int(fixed_pt x, int n){
    return x + convert_to_fixed(n);
}

//Subtraction of two fixed point numbers
fixed_pt sub_fixed(fixed_pt x, fixed_pt y){
    return x - y;
}

//Subtraction of fixed point and integer
fixed_pt sub_fixed_int(fixed_pt x, int n){
    return x - convert_to_fixed(n);
}

//Multiplication of two fixed point numbers
fixed_pt mul_fixed(fixed_pt x, fixed_pt y){
    return ((int64_t)x) * y / f;
}

//Multiplication of fixed point and integer
fixed_pt mul_fixed_int(fixed_pt x, int n){
    return x * n;
}

//Division of two fixed point numbers
fixed_pt div_fixed(fixed_pt x, fixed_pt y){
    return ((int64_t)x) * f / y;
}

//Division of fixed point and integer
fixed_pt div_fixed_int(fixed_pt x, int n){
    return x / n;
}
