# sl-vector
### _GLSL-style vector types, casts and functions in C23_ 

sl-vector is a bunch of _Generic macro's for vector (linear algebra things) types, including _smart_ type castings, geometry functions and "common math.h but component-wise"

## Examples
```C
vec3 v1 =       vec4(1, 2.9, 3.9f, 4).xyz;  // v1 is {1.0f, 2.9f, 3.9f}
vec3 v2 = vec4(ivec4(1, 2.9, 3.9f, 4)).xyz; // v2 is {1.0f, 2.0f, 3.0f} because of to-int intermediate casting

ivec2 v3 = ivec3(1,2,3).yz; //v3 is {2, 3}
bvec3 v4 = equal(ivec3(2,3,4), ivec2(2,3,5)); // v4 is {1, 1, 0}
bool  b1 = any(v4); //1, at least one component is 1
bool  b2 = all(v4); //0, at least one component is 0
```
