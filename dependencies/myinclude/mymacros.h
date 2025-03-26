#ifndef _MY_MACROS_H
#define _MY_MACROS_H

#define uint unsigned int
#define ADD(a, b) (vec3){ a.x + b.x, a.y + b.y , a.z + b.z}
#define ADD2i(a, b) (ivec2){ a.x + b.x, a.y + b.y }
#define SCA(s, a) (vec3){ s * a.x, s * a.y , s * a.z }
#define SCA2i(s, a) (ivec2){ s * a.x, s * a.y}
#define SIGN(s) (s < 0) ? -1.0f : 1.0f

#endif
