#pragma once

#ifndef SL_VEC_INCLUDE_GUARD
#define SL_VEC_INCLUDE_GUARD

#include <tgmath.h>
#include <stdbool.h>
/*
C23 standart required (might work without)
"templates in C"
This header provides GLSL-style castings (for limited types) and some functions
example:
vec4 my_vector = vec4(ivec3(7,6.66,5.0f), 1.0); //my_vector is {7.0, 6.0, 5.0, 1.0}

things like
vec3 v3 = v4.xyz; 
or
vec3 v3 = v4.yzw; 
are supported 

things like
vec3 v3 = v4.xzy; 
or
vec3 v3 = v4.xxx; 
are not supported

list of supported swizzle's:
.x, .y, .z, .w; for 2d, 3d, 4d vectors
.xy, .yz, .zw; for 2d, 3d, 4d vectors
.xyz, .yzw; for 4d vectors
could be supported but why would you want to write v3.xyz when its same as v3?

If someone knows how to manually align parts of struct in union, please tell me (so vec.xxx could be used)
*/

//creating of vector types. Extra type for .xy for vec2
#define _create_vec_types(type, prefix) \
typedef union _##prefix##vec2 {  \
    type m[2];                  \
    struct {type x, y;};        \
} _##prefix##vec2;          \
typedef union prefix##vec2 {  \
    type m[2];                  \
    struct {type x, y;};        \
    _##prefix##vec2 xy;       \
} prefix##vec2;               \
typedef union prefix##vec3 {  \
    type m[3];                  \
    struct {type x, y, z;};     \
    struct {prefix##vec2 xy; type _z;};     \
    struct {type _x; prefix##vec2 yz;};     \
} prefix##vec3;               \
typedef union prefix##vec4 {  \
    type m[4];                  \
    struct {type x, y, z, w;};  \
    struct {prefix##vec3 xyz; type _w;};     \
    struct {type _x; prefix##vec3 yzw;};     \
    struct {prefix##vec2 xy; type _z; type __w;};     \
    struct {type __x; prefix##vec2 yz; type ___w;};     \
    struct {type ___x; type _y; prefix##vec2 zw;};     \
} prefix##vec4;

_create_vec_types(   int, i)
_create_vec_types( float,  )
_create_vec_types(double, d)
_create_vec_types(  bool, b)

#define GET_MACRO(_0, _1, _2, _3, _4, NAME, ...) NAME
#define CONCAT(x,y) x ## y
#define _1arg_wrapper(x) x
#define _2arg_wrapper(x,y) x,y
#define _3arg_wrapper(x,y,z) x,y,z
#define _create_type_error() _assert("WRONG TYPE IN VECTOR CONSTRUCTOR\n", __FILE__, __LINE__)

#define _expand_4types_helper(type1, type2, type3, type4, macro, first_arg, ...) \
    type1 : macro(first_arg, type1, __VA_ARGS__),\
    type2 : macro(first_arg, type2, __VA_ARGS__),\
    type3 : macro(first_arg, type3, __VA_ARGS__),\
    type4 : macro(first_arg, type4, __VA_ARGS__)
#define _expand_4types(type1, type2, type3, type4, macro, first_arg, ...) \
    type1 : macro(first_arg, type1, __VA_ARGS__),\
    type2 : macro(first_arg, type2, __VA_ARGS__),\
    type3 : macro(first_arg, type3, __VA_ARGS__),\
    type4 : macro(first_arg, type4, __VA_ARGS__)
/*======================================================*/
#define _create_Generic_1d_2args(var, macro, type, ...) \
_Generic((var),\
    _expand_4types(int, float, double, bool, macro, type, __VA_ARGS__),\
    default : ({_create_type_error();})\
)
#define _create_Generic_2d_2args(var, macro, type, ...) \
_Generic((var),\
    _expand_4types( ivec2,  vec2,  dvec2,  bvec2, macro, type, __VA_ARGS__),\
    _expand_4types(_ivec2, _vec2, _dvec2, _bvec2, macro, type, __VA_ARGS__),\
    default : ({_create_type_error();})\
)
#define _create_Generic_3d_2args(var, macro, type, ...) \
_Generic((var),\
    _expand_4types(ivec3, vec3, dvec3, bvec3, macro, type, __VA_ARGS__),\
    default : ({_create_type_error();})\
)
#define _create_Generic_1d_3args(var, macro, _type1, _type2, ...) \
_Generic((var),\
    _expand_4types(int, float, double, bool, macro, _2arg_wrapper(_type1, _type2), __VA_ARGS__),\
    default : ({_create_type_error();})\
)
#define _create_Generic_2d_3args(var, macro, _type1, _type2, ...) \
_Generic((var),\
    _expand_4types( ivec2,  vec2,  dvec2,  bvec2, macro, _2arg_wrapper(_type1, _type2), __VA_ARGS__),\
    _expand_4types(_ivec2, _vec2, _dvec2, _bvec2, macro, _2arg_wrapper(_type1, _type2), __VA_ARGS__),\
    default : ({_create_type_error();})\
)

#define _vector_xy( prefix, x, y) ( (prefix##vec2){x, y} )
#define _vector_vec(prefix, vec ) ( _vector_xy(prefix, vec.x, vec.y) )
#define _vector2d(prefix, ...) GET_MACRO(_0, __VA_ARGS__, ERROR4ARGS, ERROR3ARGS, _vector_xy, _vector_vec, ERROR0ARGS)(prefix, __VA_ARGS__)
/*---------------------------------------------*/
#define ivec2(...) _vector2d(i, __VA_ARGS__)
#define  vec2(...) _vector2d( , __VA_ARGS__)
#define dvec2(...) _vector2d(d, __VA_ARGS__)
#define bvec2(...) _vector2d(b, __VA_ARGS__)



#define _vec3_xyz(prefix, x,y,z) ( (prefix##vec3){x, y, z} )  
#define _vec3_vec(prefix, vec  ) ( _vec3_xyz(prefix, vec.x, vec.y, vec.z) )

#define _3d_from_1dN2d(a_type, b_type, vec, a_p, b_p) \
({vec = (typeof(vec)) {*(a_type*)a_p, (*(b_type*)b_p).x, (*(b_type*)b_p).y};})
/*------------------------------------------------------------------*/
#define _3d_from_2dN1d(a_type, b_type, vec, a_p, b_p) \
({vec = (typeof(vec)) {(*(a_type*)a_p).x, (*(a_type*)a_p).y, *(b_type*)b_p};})

//This has to be done through pointer because of compile-time nature of _Generic
#define _vec3_2args_wrapper(prefix, a, b) \
({prefix##vec3 vec;\
typeof(a) a_copy = a; typeof(a)* a_p = &a_copy;\
typeof(b) b_copy = b; typeof(b)* b_p = &b_copy;\
_Generic ((a),\
    _expand_4types_helper(int, float, double, bool, _create_Generic_2d_2args, _2arg_wrapper(b, _3d_from_1dN2d), vec, a_p, b_p),\
    _expand_4types_helper(ivec2, vec2, dvec2, bvec2, _create_Generic_1d_2args, _2arg_wrapper(b, _3d_from_2dN1d), vec, a_p, b_p),\
    default : ({_create_type_error();})\
);\
vec;})

#define _vector3d(prefix, ...) GET_MACRO(_0, __VA_ARGS__, ERROR4ARGS, _vec3_xyz, _vec3_2args_wrapper, _vec3_vec, ERROR0ARGS)(prefix, __VA_ARGS__)

#define ivec3(...) _vector3d(i, __VA_ARGS__)
#define  vec3(...) _vector3d( , __VA_ARGS__)
#define dvec3(...) _vector3d(d, __VA_ARGS__)
#define bvec3(...) _vector3d(b, __VA_ARGS__)



#define _vec4_xyzw(prefix, x,y,z,w) ( (prefix##vec4){x, y, z, w} )
#define _vec4_vec( prefix, vec    ) ( _vec4_xyzw(prefix, vec.x, vec.y, vec.z, vec.w) )

#define _4d_from_1dN3d(a_type, b_type, vec, a_p, b_p) \
({vec = (typeof(vec)) {*(a_type*)a_p, (*(b_type*)b_p).x, (*(b_type*)b_p).y, (*(b_type*)b_p).z};})
/*------------------------------------------------------------------*/
#define _4d_from_2dN2d(a_type, b_type, vec, a_p, b_p) \
({vec = (typeof(vec)) {(*(a_type*)a_p).x, (*(a_type*)a_p).y, (*(b_type*)b_p).x, (*(b_type*)b_p).y};})
/*------------------------------------------------------------------*/
#define _4d_from_3dN1d(a_type, b_type, vec, a_p, b_p) \
({vec = (typeof(vec)) {(*(a_type*)a_p).x, (*(a_type*)a_p).y, (*(a_type*)a_p).z, *(b_type*)b_p};})
/*------------------------------------------------------------------*/

#define _vec4_2args_wrapper(prefix, a, b) \
({prefix##vec4 vec;\
typeof(a) a_copy = a; typeof(a)* a_p = &a_copy;\
typeof(b) b_copy = b; typeof(b)* b_p = &b_copy;\
_Generic ((a),\
    _expand_4types_helper(int, float, double, bool, _create_Generic_3d_2args, _2arg_wrapper(b, _4d_from_1dN3d), vec, a_p, b_p),\
    _expand_4types_helper(ivec2, vec2, dvec2, bvec2, _create_Generic_2d_2args, _2arg_wrapper(b, _4d_from_2dN2d), vec, a_p, b_p),\
    _expand_4types_helper(ivec3, vec3, dvec3, bvec3, _create_Generic_1d_2args, _2arg_wrapper(b, _4d_from_3dN1d), vec, a_p, b_p),\
    default : ({_create_type_error();})\
);\
vec;})

#define _4d_from_1dN1dN2d(a_type, b_type, c_type, vec, a_p, b_p, c_p) \
({vec = (typeof(vec)) {*(a_type*)a_p, *(b_type*)b_p, (*(c_type*)c_p).x, (*(c_type*)c_p).y};})
#define _4d_from_1dN2dN1d(a_type, b_type, c_type, vec, a_p, b_p, c_p) \
({vec = (typeof(vec)) {*(a_type*)a_p, (*(b_type*)b_p).x, (*(b_type*)b_p).y, *(c_type*)c_p};})
#define _4d_from_2dN1dN1d(a_type, b_type, c_type, vec, a_p, b_p, c_p) \
({vec = (typeof(vec)) {(*(a_type*)a_p).x, (*(a_type*)a_p).y, *(b_type*)b_p, *(c_type*)c_p};})

#define _vec4_3args_wrapper(prefix, a, b, c) \
({prefix##vec4 vec;\
typeof(a) a_copy = a; typeof(a)* a_p = &a_copy;\
typeof(b) b_copy = b; typeof(b)* b_p = &b_copy;\
typeof(c) c_copy = c; typeof(c)* c_p = &c_copy;\
_Generic ((a),\
    int     : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_2d_3args, _3arg_wrapper(c, _4d_from_1dN1dN2d, int), vec, a_p, b_p, c_p),\
        _expand_4types_helper( ivec2,  vec2,  dvec2,  bvec2, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_1dN2dN1d, int), vec, a_p, b_p, c_p),\
        _expand_4types_helper(_ivec2, _vec2, _dvec2, _bvec2, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_1dN2dN1d, int), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    float : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_2d_3args, _3arg_wrapper(c, _4d_from_1dN1dN2d, float), vec, a_p, b_p, c_p),\
        _expand_4types_helper( ivec2,  vec2,  dvec2,  bvec2, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_1dN2dN1d, float), vec, a_p, b_p, c_p),\
        _expand_4types_helper(_ivec2, _vec2, _dvec2, _bvec2, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_1dN2dN1d, float), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    double : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_2d_3args, _3arg_wrapper(c, _4d_from_1dN1dN2d, double), vec, a_p, b_p, c_p),\
        _expand_4types_helper( ivec2,  vec2,  dvec2,  bvec2, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_1dN2dN1d, double), vec, a_p, b_p, c_p),\
        _expand_4types_helper(_ivec2, _vec2, _dvec2, _bvec2, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_1dN2dN1d, double), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    ivec2 : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_2dN1dN1d, ivec2), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    _ivec2 : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_2dN1dN1d, _ivec2), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
     vec2 : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_2dN1dN1d, vec2), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    _vec2 : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_2dN1dN1d, _vec2), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    dvec2 : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_2dN1dN1d, dvec2), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    _dvec2 : _Generic((b),\
        _expand_4types_helper(int, float, double, bool, _create_Generic_1d_3args, _3arg_wrapper(c, _4d_from_2dN1dN1d, _dvec2), vec, a_p, b_p, c_p),\
        default : ({_create_type_error();})\
    ),\
    default : ({_create_type_error();})\
);\
vec;})

#define _vector4d(prefix, ...) GET_MACRO(_0, __VA_ARGS__, _vec4_xyzw, _vec4_3args_wrapper, _vec4_2args_wrapper, _vec4_vec, ERROR0ARGS)(prefix, __VA_ARGS__)

#define ivec4(...) _vector4d(i, __VA_ARGS__)
#define  vec4(...) _vector4d( , __VA_ARGS__)
#define dvec4(...) _vector4d(d, __VA_ARGS__)
#define bvec4(...) _vector4d(b, __VA_ARGS__)


/*
Part with actual functions
Functions are not simple macro and actual _Generic function calls 
this is because of you might not want everything to be "inlined" as it is with macro
*/

int _fun_mod_int (int a, int b) {return a % b;}
bool _fun_mod_bool (bool a, bool b) {return a % b;}
float _fun_mod_float (float a, float b) {return fmodf(a,b);} //can be replaced with fmod(a,b) 
double _fun_mod_double (double a, double b) {return fmod(a,b);}

int _fun_floor_int (int var) {return var;}
bool _fun_floor_bool (bool var) {return var;}
float _fun_floor_float (float var) {return floorf(var);}
double _fun_floor_double (double var) {return floorl(var);} //TODO : fix this with propper macro thing

int _fun_ceil_int (int var) {return var;}
bool _fun_ceil_bool (bool var) {return var;}
float _fun_ceil_float (float var) {return ceilf(var);}
double _fun_ceil_double (double var) {return ceilf(var);}  //TODO : fix this with propper macro thing

int _fun_round_int (int var) {return var;}
bool _fun_round_bool (bool var) {return var;}
float _fun_round_float (float var) {return roundf(var);}
double _fun_round_double (double var) {return round(var);}  //TODO : fix this with propper macro thing

#define _create_1d_fun_sign(type) \
    type _fun_sign_ ##type (type var) {return var < 0 ? -1 : +1;}
#define _create_1d_fun_min(type) \
    type _fun_min_ ##type (type a, type b)   {return a < b ? a : b;}
#define _create_1d_fun_max(type) \
    type _fun_max_ ##type (type a, type b)   {return a < b ? b : a;}
#define _create_1d_fun_abs(type) \
    type _fun_abs_ ##type (type var)   {return var < 0 ? -var : +var;}


#define _create_1d_funs(name) \
_create_1d_fun_##name(int)   \
_create_1d_fun_##name(float) \
_create_1d_fun_##name(double)\
_create_1d_fun_##name(bool)

#define _call_1d_fun(name,val, ...) \
_Generic ((val),\
    int    : _fun_ ##name## _int,\
    float  : _fun_ ##name## _float,\
    double : _fun_ ##name## _double,\
    bool   : _fun_ ##name## _bool,\
    default : ({_create_type_error();})\
)(val, __VA_ARGS__)

_create_1d_funs(sign)
_create_1d_funs(min)
_create_1d_funs(max)
_create_1d_funs(abs)

#define _create_fun_add(prefix) \
prefix##vec2 _fun_add_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (prefix##vec2) {a.x+b.x, a.y+b.y}                  ;}\
prefix##vec3 _fun_add_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (prefix##vec3) {a.x+b.x, a.y+b.y, a.z+b.z}         ;}\
prefix##vec4 _fun_add_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (prefix##vec4) {a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};}

#define _create_fun_sub(prefix) \
prefix##vec2 _fun_sub_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (prefix##vec2) {a.x-b.x, a.y-b.y}                  ;}\
prefix##vec3 _fun_sub_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (prefix##vec3) {a.x-b.x, a.y-b.y, a.z-b.z}         ;}\
prefix##vec4 _fun_sub_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (prefix##vec4) {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};}

#define _create_fun_mul(prefix) \
prefix##vec2 _fun_mul_##prefix##vec2(prefix##vec2 a, prefix##vec2 b) {return (prefix##vec2) {a.x*b.x, a.y*b.y}                  ;}\
prefix##vec3 _fun_mul_##prefix##vec3(prefix##vec3 a, prefix##vec3 b) {return (prefix##vec3) {a.x*b.x, a.y*b.y, a.z*b.z}         ;}\
prefix##vec4 _fun_mul_##prefix##vec4(prefix##vec4 a, prefix##vec4 b) {return (prefix##vec4) {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};}\
prefix##vec2 _fun_mul_##prefix##vec2_scale(prefix##vec2 a, typeof((prefix##vec2){0}.x) b)  {return (prefix##vec2) {a.x*b, a.y*b}              ;}\
prefix##vec3 _fun_mul_##prefix##vec3_scale(prefix##vec3 a, typeof((prefix##vec3){0}.x) b)  {return (prefix##vec3) {a.x*b, a.y*b, a.z*b}       ;}\
prefix##vec4 _fun_mul_##prefix##vec4_scale(prefix##vec4 a, typeof((prefix##vec4){0}.x) b)  {return (prefix##vec4) {a.x*b, a.y*b, a.z*b, a.w*b};}

#define _create_fun_div(prefix) \
prefix##vec2 _fun_div_##prefix##vec2(prefix##vec2 a, prefix##vec2 b) {return (prefix##vec2) {a.x/b.x, a.y/b.y}                  ;}\
prefix##vec3 _fun_div_##prefix##vec3(prefix##vec3 a, prefix##vec3 b) {return (prefix##vec3) {a.x/b.x, a.y/b.y, a.z/b.z}         ;}\
prefix##vec4 _fun_div_##prefix##vec4(prefix##vec4 a, prefix##vec4 b) {return (prefix##vec4) {a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w};}\
prefix##vec2 _fun_div_##prefix##vec2_scale(prefix##vec2 a, typeof((prefix##vec2){0}.x) b)  {return (prefix##vec2) {a.x/b, a.y/b}              ;}\
prefix##vec3 _fun_div_##prefix##vec3_scale(prefix##vec3 a, typeof((prefix##vec3){0}.x) b)  {return (prefix##vec3) {a.x/b, a.y/b, a.z/b}       ;}\
prefix##vec4 _fun_div_##prefix##vec4_scale(prefix##vec4 a, typeof((prefix##vec4){0}.x) b)  {return (prefix##vec4) {a.x/b, a.y/b, a.z/b, a.w/b};}


#define _create_fun_cross(prefix) \
prefix##vec3 _fun_cross_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (prefix##vec3) {a.y*b.z - b.y*a.z,  \
                                                                                                a.z*b.x - b.z*a.x,  \
                                                                                                a.x*b.y - b.x*a.y};}\

#define _create_fun_dot(prefix) \
typeof((prefix##vec2){0}.x) _fun_dot_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (a.x*b.x + a.y*b.y)                    ;}\
typeof((prefix##vec3){0}.x) _fun_dot_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (a.x*b.x + a.y*b.y + a.z*b.z)          ;}\
typeof((prefix##vec4){0}.x) _fun_dot_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);}

#define _create_fun_length(prefix) \
typeof((prefix##vec2){0}.x) _fun_length_##prefix##vec2(prefix##vec2 vec)  {return sqrt(_fun_dot_##prefix##vec2(vec, vec));}\
typeof((prefix##vec3){0}.x) _fun_length_##prefix##vec3(prefix##vec3 vec)  {return sqrt(_fun_dot_##prefix##vec3(vec, vec));}\
typeof((prefix##vec4){0}.x) _fun_length_##prefix##vec4(prefix##vec4 vec)  {return sqrt(_fun_dot_##prefix##vec4(vec, vec));}

#define _create_fun_distance(prefix) \
typeof((prefix##vec2){0}.x) _fun_distance_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return _fun_length_##prefix##vec2(_fun_sub_##prefix##vec2(a,b));}\
typeof((prefix##vec3){0}.x) _fun_distance_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return _fun_length_##prefix##vec3(_fun_sub_##prefix##vec3(a,b));}\
typeof((prefix##vec4){0}.x) _fun_distance_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return _fun_length_##prefix##vec4(_fun_sub_##prefix##vec4(a,b));}\

#define _create_fun_normilize(prefix) \
prefix##vec2 _fun_normilize_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (prefix##vec2) {a.x-b.x, a.y-b.y}                  ;}\
prefix##vec3 _fun_normilize_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (prefix##vec3) {a.x-b.x, a.y-b.y, a.z-b.z}         ;}\
prefix##vec4 _fun_normilize_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (prefix##vec4) {a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};}

#define _create_fun_lessThan(prefix) \
bvec2 _fun_lessThan_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (bvec2) {a.x<b.x, a.y<b.y}                  ;}\
bvec3 _fun_lessThan_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (bvec3) {a.x<b.x, a.y<b.y, a.z<b.z}         ;}\
bvec4 _fun_lessThan_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (bvec4) {a.x<b.x, a.y<b.y, a.z<b.z, a.w<b.w};}

#define _create_fun_lessThanEqual(prefix) \
bvec2 _fun_lessThanEqual_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (bvec2) {a.x<=b.x, a.y<=b.y}                  ;}\
bvec3 _fun_lessThanEqual_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (bvec3) {a.x<=b.x, a.y<=b.y, a.z<=b.z}         ;}\
bvec4 _fun_lessThanEqual_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (bvec4) {a.x<=b.x, a.y<=b.y, a.z<=b.z, a.w<=b.w};}

#define _create_fun_greaterThan(prefix) \
bvec2 _fun_greaterThan_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (bvec2) {a.x>b.x, a.y>b.y}                  ;}\
bvec3 _fun_greaterThan_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (bvec3) {a.x>b.x, a.y>b.y, a.z>b.z}         ;}\
bvec4 _fun_greaterThan_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (bvec4) {a.x>b.x, a.y>b.y, a.z>b.z, a.w>b.w};}

#define _create_fun_greaterThanEqual(prefix) \
bvec2 _fun_greaterThanEqual_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (bvec2) {a.x>=b.x, a.y>=b.y}                    ;}\
bvec3 _fun_greaterThanEqual_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (bvec3) {a.x>=b.x, a.y>=b.y, a.z>=b.z}          ;}\
bvec4 _fun_greaterThanEqual_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (bvec4) {a.x>=b.x, a.y>=b.y, a.z>=b.z, a.w>=b.w};}

#define _create_fun_equal(prefix) \
bvec2 _fun_equal_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (bvec2) {a.x==b.x, a.y==b.y}                    ;}\
bvec3 _fun_equal_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (bvec3) {a.x==b.x, a.y==b.y, a.z==b.z}          ;}\
bvec4 _fun_equal_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (bvec4) {a.x==b.x, a.y==b.y, a.z==b.z, a.w==b.w};}

#define _create_fun_notEqual(prefix) \
bvec2 _fun_notEqual_##prefix##vec2(prefix##vec2 a, prefix##vec2 b)  {return (bvec2) {a.x!=b.x, a.y!=b.y}                    ;}\
bvec3 _fun_notEqual_##prefix##vec3(prefix##vec3 a, prefix##vec3 b)  {return (bvec3) {a.x!=b.x, a.y!=b.y, a.z!=b.z}          ;}\
bvec4 _fun_notEqual_##prefix##vec4(prefix##vec4 a, prefix##vec4 b)  {return (bvec4) {a.x!=b.x, a.y!=b.y, a.z!=b.z, a.w!=b.w};}

#define _create_fun_any(prefix) \
bool _fun_any_##prefix##vec2(prefix##vec2 vec)  {return (vec.x || vec.y)                  ;}\
bool _fun_any_##prefix##vec3(prefix##vec3 vec)  {return (vec.x || vec.y || vec.z)         ;}\
bool _fun_any_##prefix##vec4(prefix##vec4 vec)  {return (vec.x || vec.y || vec.z || vec.w);}

#define _create_fun_all(prefix) \
bool _fun_all_##prefix##vec2(prefix##vec2 vec)  {return (vec.x && vec.y)                  ;}\
bool _fun_all_##prefix##vec3(prefix##vec3 vec)  {return (vec.x && vec.y && vec.z)         ;}\
bool _fun_all_##prefix##vec4(prefix##vec4 vec)  {return (vec.x && vec.y && vec.z && vec.w);}

#define _create_fun_not(prefix) \
bvec2 _fun_not_##prefix##vec2(prefix##vec2 vec)  {return (bvec2) {!vec.x, !vec.y}                ;}\
bvec3 _fun_not_##prefix##vec3(prefix##vec3 vec)  {return (bvec3) {!vec.x, !vec.y, !vec.z}        ;}\
bvec4 _fun_not_##prefix##vec4(prefix##vec4 vec)  {return (bvec4) {!vec.x, !vec.y, !vec.z, !vec.w};}

#define _create_fun_abs(prefix, type) \
prefix##vec2 _fun_abs_##prefix##vec2(prefix##vec2 vec) {return (prefix##vec2) {(_fun_abs_ ## type) (vec.x), (_fun_abs_ ## type) (vec.y)}                                                      ;} \
prefix##vec3 _fun_abs_##prefix##vec3(prefix##vec3 vec) {return (prefix##vec3) {(_fun_abs_ ## type) (vec.x), (_fun_abs_ ## type) (vec.y), (_fun_abs_ ## type) (vec.z)}                           ;} \
prefix##vec4 _fun_abs_##prefix##vec4(prefix##vec4 vec) {return (prefix##vec4) {(_fun_abs_ ## type) (vec.x), (_fun_abs_ ## type) (vec.y), (_fun_abs_ ## type) (vec.z), (_fun_abs_ ## type) (vec.w)};}

#define _create_fun_sign(prefix, type) \
prefix##vec2 _fun_sign_##prefix##vec2(prefix##vec2 v) { return (prefix##vec2) { copysign(1, v.x), copysign(1, v.y) }                                    ;} \
prefix##vec3 _fun_sign_##prefix##vec3(prefix##vec3 v) { return (prefix##vec3) { copysign(1, v.x), copysign(1, v.y), copysign(1, v.z) }                  ;} \
prefix##vec4 _fun_sign_##prefix##vec4(prefix##vec4 v) { return (prefix##vec4) { copysign(1, v.x), copysign(1, v.y), copysign(1, v.z), copysign(1, v.w) };}

#define _create_fun_floor(prefix, type) \
prefix##vec2 _fun_floor_##prefix##vec2(prefix##vec2 v) { return (prefix##vec2) {(_fun_floor_ ## type) (v.x), (_fun_floor_ ## type) (v.y) }                                                      ;} \
prefix##vec3 _fun_floor_##prefix##vec3(prefix##vec3 v) { return (prefix##vec3) {(_fun_floor_ ## type) (v.x), (_fun_floor_ ## type) (v.y), (_fun_floor_ ## type) (v.z) }                           ;} \
prefix##vec4 _fun_floor_##prefix##vec4(prefix##vec4 v) { return (prefix##vec4) {(_fun_floor_ ## type) (v.x), (_fun_floor_ ## type) (v.y), (_fun_floor_ ## type) (v.z), (_fun_floor_ ## type) (v.w) };}

#define _create_fun_ceil(prefix, type) \
prefix##vec2 _fun_ceil_##prefix##vec2(prefix##vec2 v) { return (prefix##vec2) { (_fun_ceil_ ## type) (v.x), (_fun_ceil_ ## type) (v.y) }                                                    ;} \
prefix##vec3 _fun_ceil_##prefix##vec3(prefix##vec3 v) { return (prefix##vec3) { (_fun_ceil_ ## type) (v.x), (_fun_ceil_ ## type) (v.y), (_fun_ceil_ ## type) (v.z) }                          ;} \
prefix##vec4 _fun_ceil_##prefix##vec4(prefix##vec4 v) { return (prefix##vec4) { (_fun_ceil_ ## type) (v.x), (_fun_ceil_ ## type) (v.y), (_fun_ceil_ ## type) (v.z), (_fun_ceil_ ## type) (v.w) };}

#define _create_fun_round(prefix, type) \
prefix##vec2 _fun_round_##prefix##vec2(prefix##vec2 v) { return (prefix##vec2) { (_fun_round_ ## type) (v.x), (_fun_round_ ## type) (v.y) }                                                    ;} \
prefix##vec3 _fun_round_##prefix##vec3(prefix##vec3 v) { return (prefix##vec3) { (_fun_round_ ## type) (v.x), (_fun_round_ ## type) (v.y), (_fun_round_ ## type) (v.z) }                          ;} \
prefix##vec4 _fun_round_##prefix##vec4(prefix##vec4 v) { return (prefix##vec4) { (_fun_round_ ## type) (v.x), (_fun_round_ ## type) (v.y), (_fun_round_ ## type) (v.z), (_fun_round_ ## type) (v.w) };}

#define _create_fun_mod(prefix, type) \
prefix##vec2 _fun_mod_##prefix##vec2(prefix##vec2 v1, prefix##vec2 v2) { return (prefix##vec2) { (_fun_mod_ ## type) (v1.x, v2.x), (_fun_mod_ ## type) (v1.y, v2.y) }                                                                ;} \
prefix##vec3 _fun_mod_##prefix##vec3(prefix##vec3 v1, prefix##vec3 v2) { return (prefix##vec3) { (_fun_mod_ ## type) (v1.x, v2.x), (_fun_mod_ ## type) (v1.y, v2.y), (_fun_mod_ ## type) (v1.z, v2.z) }                                ;} \
prefix##vec4 _fun_mod_##prefix##vec4(prefix##vec4 v1, prefix##vec4 v2) { return (prefix##vec4) { (_fun_mod_ ## type) (v1.x, v2.x), (_fun_mod_ ## type) (v1.y, v2.y), (_fun_mod_ ## type) (v1.z, v2.z), (_fun_mod_ ## type) (v1.w, v2.w) };}

#define _create_fun_min(prefix, type) \
prefix##vec2 _fun_min_##prefix##vec2(prefix##vec2 v1, prefix##vec2 v2) { return (prefix##vec2) { (_fun_min_ ## type) (v1.x, v2.x), (_fun_min_ ## type) (v1.y, v2.y) }                                                                ;} \
prefix##vec3 _fun_min_##prefix##vec3(prefix##vec3 v1, prefix##vec3 v2) { return (prefix##vec3) { (_fun_min_ ## type) (v1.x, v2.x), (_fun_min_ ## type) (v1.y, v2.y), (_fun_min_ ## type) (v1.z, v2.z) }                                ;} \
prefix##vec4 _fun_min_##prefix##vec4(prefix##vec4 v1, prefix##vec4 v2) { return (prefix##vec4) { (_fun_min_ ## type) (v1.x, v2.x), (_fun_min_ ## type) (v1.y, v2.y), (_fun_min_ ## type) (v1.z, v2.z), (_fun_min_ ## type) (v1.w, v2.w) };}

#define _create_fun_max(prefix, type) \
prefix##vec2 _fun_max_##prefix##vec2(prefix##vec2 v1, prefix##vec2 v2) { return (prefix##vec2) { (_fun_max_ ## type) (v1.x, v2.x), (_fun_max_ ## type) (v1.y, v2.y) }                                                                ;} \
prefix##vec3 _fun_max_##prefix##vec3(prefix##vec3 v1, prefix##vec3 v2) { return (prefix##vec3) { (_fun_max_ ## type) (v1.x, v2.x), (_fun_max_ ## type) (v1.y, v2.y), (_fun_max_ ## type) (v1.z, v2.z) }                                ;} \
prefix##vec4 _fun_max_##prefix##vec4(prefix##vec4 v1, prefix##vec4 v2) { return (prefix##vec4) { (_fun_max_ ## type) (v1.x, v2.x), (_fun_max_ ## type) (v1.y, v2.y), (_fun_max_ ## type) (v1.z, v2.z), (_fun_max_ ## type) (v1.w, v2.w) };}

#define _create_funs(name) \
_create_fun_##name(i)\
_create_fun_##name( )\
_create_fun_##name(d)\
_create_fun_##name(b)

#define _create_funs_compwise(name) \
_create_fun_##name(i, int)\
_create_fun_##name( , float)\
_create_fun_##name(d, double)\
_create_fun_##name(b, bool)

_create_funs_compwise(abs)
_create_funs_compwise(min)
_create_funs_compwise(max)
_create_funs_compwise(mod)
_create_funs_compwise(floor)
_create_funs_compwise(ceil)
_create_funs_compwise(round)

_create_funs(add)
_create_funs(sub)
_create_funs(mul)
_create_funs(div)
_create_funs(dot)
_create_funs(cross)
_create_funs(length)
_create_funs(distance)
_create_funs(normilize)

_create_funs(lessThan)
_create_funs(lessThanEqual)
_create_funs(greaterThan)
_create_funs(greaterThanEqual)
_create_funs(equal)
_create_funs(notEqual)

// _create_funs(notEqual)
_create_fun_any(b)
_create_fun_all(b)
_create_fun_not(b)

#define _call_fun(name, vec, ...) \
_Generic((vec),\
   _ivec2 : _fun_##name##_ivec2,\
    ivec2 : _fun_##name##_ivec2,\
    ivec3 : _fun_##name##_ivec3,\
    ivec4 : _fun_##name##_ivec4,\
\
    _vec2 : _fun_##name##_vec2,\
     vec2 : _fun_##name##_vec2,\
     vec3 : _fun_##name##_vec3,\
     vec4 : _fun_##name##_vec4,\
\
   _dvec2 : _fun_##name##_dvec2,\
    dvec2 : _fun_##name##_dvec2,\
    dvec3 : _fun_##name##_dvec3,\
    dvec4 : _fun_##name##_dvec4,\
\
   _bvec2 : _fun_##name##_bvec2,\
    bvec2 : _fun_##name##_bvec2,\
    bvec3 : _fun_##name##_bvec3,\
    bvec4 : _fun_##name##_bvec4,\
\
    default : ({_create_type_error();})\
)(vec, ## __VA_ARGS__)

#define _call_fun_3d_only(name, vec, ...) \
_Generic((vec),\
    ivec3 : _fun_##name##_ivec3,\
\
     vec3 : _fun_##name##_vec3,\
\
    dvec3 : _fun_##name##_dvec3,\
\
    default : ({_create_type_error();})\
)(vec, ## __VA_ARGS__)


#define _call_fun_bool_only(name, vec, ...) \
_Generic((vec),\
   _bvec2 : _fun_##name##_bvec2,\
    bvec2 : _fun_##name##_bvec2,\
    bvec3 : _fun_##name##_bvec3,\
    bvec4 : _fun_##name##_bvec4,\
\
    default : ({_create_type_error();})\
)(vec, ## __VA_ARGS__)

#define _call_fun_vecBYvecORvecBYscale(name, vec, second_arg) \
_Generic((second_arg),\
   _ivec2 : _fun_##name##_ivec2,\
    ivec2 : _fun_##name##_ivec2,\
    ivec3 : _fun_##name##_ivec3,\
    ivec4 : _fun_##name##_ivec4,\
\
    _vec2 : _fun_##name##_vec2,\
     vec2 : _fun_##name##_vec2,\
     vec3 : _fun_##name##_vec3,\
     vec4 : _fun_##name##_vec4,\
\
   _dvec2 : _fun_##name##_dvec2,\
    dvec2 : _fun_##name##_dvec2,\
    dvec3 : _fun_##name##_dvec3,\
    dvec4 : _fun_##name##_dvec4,\
\
    int : _Generic((vec),\
       _ivec2 : _fun_##name##_ivec2_scale,\
        ivec2 : _fun_##name##_ivec2_scale,\
        ivec3 : _fun_##name##_ivec3_scale,\
        ivec4 : _fun_##name##_ivec4_scale,\
        default : ({_create_type_error();})\
    ),\
    float : _Generic((vec),\
       _vec2 : _fun_##name##_vec2_scale,\
        vec2 : _fun_##name##_vec2_scale,\
        vec3 : _fun_##name##_vec3_scale,\
        vec4 : _fun_##name##_vec4_scale,\
        default : ({_create_type_error();})\
    ),\
    double : _Generic((vec),\
       _dvec2 : _fun_##name##_dvec2_scale,\
        dvec2 : _fun_##name##_dvec2_scale,\
        dvec3 : _fun_##name##_dvec3_scale,\
        dvec4 : _fun_##name##_dvec4_scale,\
        default : ({_create_type_error();})\
    ),\
    default : ({_create_type_error();})\
)(vec, second_arg)



#define _call_intersect_with_math(name, vec, ...) \
_Generic((vec),\
   _ivec2 : _fun_##name##_ivec2,\
    ivec2 : _fun_##name##_ivec2,\
    ivec3 : _fun_##name##_ivec3,\
    ivec4 : _fun_##name##_ivec4,\
\
    _vec2 : _fun_##name##_vec2,\
     vec2 : _fun_##name##_vec2,\
     vec3 : _fun_##name##_vec3,\
     vec4 : _fun_##name##_vec4,\
\
   _dvec2 : _fun_##name##_dvec2,\
    dvec2 : _fun_##name##_dvec2,\
    dvec3 : _fun_##name##_dvec3,\
    dvec4 : _fun_##name##_dvec4,\
\
   _bvec2 : _fun_##name##_dvec2,\
    bvec2 : _fun_##name##_dvec2,\
    bvec3 : _fun_##name##_dvec3,\
    bvec4 : _fun_##name##_dvec4,\
\
    int    : _fun_##name##_int,\
    float  : _fun_##name##_float,\
    double : _fun_##name##_double,\
    bool : _fun_##name##_bool,\
    default : ({_create_type_error();})\
)(vec, ## __VA_ARGS__)


#define length(vec)    _call_fun(length,    vec)
#define add(a, b)      _call_fun(add,      a, b)
#define sub(a, b)      _call_fun(sub,      a, b)
#define dot(a, b)      _call_fun(dot,      a, b)
#define cross(a, b)    _call_fun_3d_only(cross, a, b)
#define distance(a, b) _call_fun(distance, a, b)
#define normilize(vec) _call_fun(normilize, vec)
#define mul(vec, scaler) _call_fun_vecBYvecORvecBYscale(mul, vec, scaler)
#define div(vec, scaler) _call_fun_vecBYvecORvecBYscale(div, vec, scaler)

#define lessThan(a, b)         _call_fun(lessThan,         a, b)
#define lessThanEqual(a, b)    _call_fun(lessThanEqual,    a, b)
#define greaterThan(a, b)      _call_fun(greaterThan,      a, b)
#define greaterThanEqual(a, b) _call_fun(greaterThanEqual, a, b)
#define equal(a, b)            _call_fun(equal,            a, b)
#define notEqual(a, b)         _call_fun(notEqual,         a, b)

#define any(vec) _call_fun_bool_only(any, vec)
#define all(vec) _call_fun_bool_only(all, vec)
#define not(vec) _call_fun_bool_only(not, vec)

#define sign(vec) _call_fun(sign, vec)
#define mod(vec)  _call_fun(mod , vec)
#define min(vec)  _call_fun(min , vec)
#define max(vec)  _call_fun(max , vec)
#define abs(vec)   _call_intersect_with_math(abs  , vec)

/*
This macro's are also defined in tgmath.h, so i have to undefine them and then define back
Sometimes single-component tgmath functions are used in vector ones so i have to create one more macro
*/
#undef floor
#undef ceil
#undef round
#define floor(vec) _call_intersect_with_math(floor, vec)
#define ceil(vec)  _call_intersect_with_math(ceil , vec)
#define round(vec)  _call_intersect_with_math(round , vec)


#endif // SL_VEC_INCLUDE_GUARD