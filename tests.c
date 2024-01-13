#include <stdio.h>
#include <assert.h>

#include "sl_vec.h"

/*
printf macro might be useful
*/

#define _printf_vec2d(prefix, specifier, vec) printf(specifier " " specifier "\n", (*((prefix##vec2*)(&vec))).x, (*((prefix##vec2*)(&vec))).y)
#define _printf_vec3d(prefix, specifier, vec) printf(specifier " " specifier " " specifier "\n", (*((prefix##vec3*)(&vec))).x, (*((prefix##vec3*)(&vec))).y, (*((prefix##vec3*)(&vec))).z)
#define _printf_vec4d(prefix, specifier, vec) printf(specifier " " specifier " " specifier " " specifier "\n", (*((prefix##vec4*)(&vec))).x, (*((prefix##vec4*)(&vec))).y, (*((prefix##vec4*)(&vec))).z, (*((prefix##vec4*)(&vec))).w)

#define printf_vec(vec) \
({typeof(vec) vec_copy = vec;\
_Generic((vec),\
   _ivec2 : _printf_vec2d(i, "%d", vec_copy),\
    ivec2 : _printf_vec2d(i, "%d", vec_copy),\
    ivec3 : _printf_vec3d(i, "%d", vec_copy),\
    ivec4 : _printf_vec4d(i, "%d", vec_copy),\
   _vec2  : _printf_vec2d( , "%.3f", vec_copy),\
    vec2  : _printf_vec2d( , "%.3f", vec_copy),\
    vec3  : _printf_vec3d( , "%.3f", vec_copy),\
    vec4  : _printf_vec4d( , "%.3f", vec_copy),\
   _dvec2 : _printf_vec2d(d, "%.3lf", vec_copy),\
    dvec2 : _printf_vec2d(d, "%.3lf", vec_copy),\
    dvec3 : _printf_vec3d(d, "%.3lf", vec_copy),\
    dvec4 : _printf_vec4d(d, "%.3lf", vec_copy),\
   _bvec2 : _printf_vec2d(b, "%d", vec_copy),\
    bvec2 : _printf_vec2d(b, "%d", vec_copy),\
    bvec3 : _printf_vec3d(b, "%d", vec_copy),\
    bvec4 : _printf_vec4d(b, "%d", vec_copy),\
    default : _create_type_error()\
);\
vec_copy;})

int main()
{
    bvec2 b00 = {0,0};
    bvec2 b01 = {0,1};
    bvec2 b10 = {1,0};
    bvec2 b11 = {1,1};

    assert(!any(b00));
    assert(any(not(b00)));
    assert(all(not(b00)));

    assert(all(greaterThan(vec4(2,2,2,2).xyz, vec4(1,1,1,999).xyz)));
    assert(all(equal(ivec4(2,2,2,2).yz, ivec4(-666,2,2,999).yz)));
    assert(all(notEqual(dvec4(2,2.1,2.1,2).yz, dvec3(2,2.2,2.2).yz)));

    assert(all(equal(ivec2(5,5), add(ivec2(2,2), ivec2(3,3)))));
    assert(all(equal(ivec2(4,4), mul(ivec2(2,2), 2))));
    assert(all(equal(ivec2(4,4), mul(ivec2(2,2), ivec2(2,2)))));
    assert(all(equal(ivec2(4,4), div(ivec2(8,8), 2))));
    assert(all(equal(ivec2(4,4), div(ivec2(8,8), ivec2(2,2)))));

    assert(dot(ivec2(1,1), div(ivec2(9,9), 2)) == 8);
    printf("%d\n", length(mul(ivec2(1,1), div(ivec2(9,9), 2))) );
    assert(length(mul(ivec2(1,1), div(ivec2(9,9), 2))) == 5);


    assert(all(equal(floor(vec3(1.1,2.2,3.3)), vec4(dvec3(1,2,3),666).xyz)));
    assert(all(equal(ceil(vec3(1.1,2.2,3.3)), add(vec4(dvec3(1,2,3),666).xyz, vec3(1,1,1)))));
    
    printf("test passed\n");


    return 0;
}