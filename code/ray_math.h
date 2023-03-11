#if !defined(MATH_H)
#define MATH_H
#include <math.h>
#include <stdlib.h>

union vec4_t 
{
	struct { r32 x, y, z, w; };
	struct { r32 r, g, b, a; };
	r32 data[4];
    
	r32 &operator[](int i)			{ return data[i]; }
	const r32 &operator[](int i) const	{ return data[i]; }
};

union vec3_t 
{
	struct { r32 x, y, z; };
	struct { r32 r, g, b; };
	r32 data[3];
    
	r32 &operator[](int i) { return data[i]; }
	const r32 &operator[](int i) const { return data[i]; }
};

union vec3i_t 
{
	struct { s32 x, y, z; };
	struct { s32 r, g, b; };
	s32 data[3];
    
	s32 &operator[](int i) { return data[i]; }
	const s32 &operator[](int i) const { return data[i]; }
};

internal inline vec3_t 
MakeVec3(r32 x, r32 y, r32 z) 
{
	vec3_t v = {};
    
	v[0] = x;
	v[1] = y;
	v[2] = z;
    
	return v;
}

internal inline vec3_t 
MV3(r32 x, r32 y, r32 z) 
{
	vec3_t v = {};
    
	v[0] = x;
	v[1] = y;
	v[2] = z;
    
	return v;
}

internal inline vec3_t 
operator +(vec3_t a, vec3_t b) 
{
	vec3_t v = {};
    
	v[0] = a[0] + b[0];
	v[1] = a[1] + b[1];
	v[2] = a[2] + b[2];
    
	return v;
}

internal inline vec3i_t 
operator +(vec3i_t a, vec3i_t b) 
{
	vec3i_t v = {};
    
	v[0] = a[0] + b[0];
	v[1] = a[1] + b[1];
	v[2] = a[2] + b[2];
    
	return v;
}

internal inline vec3_t 
operator -(vec3_t a) 
{
	vec3_t v = {};
    
	v[0] = -a[0]; 
	v[1] = -a[1];
	v[2] = -a[2];
    
	return v;
}

internal inline vec3i_t 
operator -(vec3i_t a) 
{
	vec3i_t v = {};
    
	v[0] = -a[0]; 
	v[1] = -a[1];
	v[2] = -a[2];
    
	return v;
}

internal inline vec3_t 
operator -(vec3_t a, vec3_t b) 
{
	vec3_t v = {};
    
	v[0] = a[0] - b[0];
	v[1] = a[1] - b[1];
	v[2] = a[2] - b[2];
    
	return v;
}

internal inline vec3i_t 
operator -(vec3i_t a, vec3i_t b) 
{
	vec3i_t v = {};
    
	v[0] = a[0] - b[0];
	v[1] = a[1] - b[1];
	v[2] = a[2] - b[2];
    
	return v;
}

internal inline vec3_t 
operator +(vec3_t a, r32 s) 
{
	vec3_t v = {};
    
	v[0] = a[0] + s;
	v[1] = a[1] + s;
	v[2] = a[2] + s;
    
	return v;
}

internal inline vec3_t 
operator -(vec3_t a, r32 s) 
{
	vec3_t v = {};
    
	v[0] = a[0] - s;
	v[1] = a[1] - s;
	v[2] = a[2] - s;
    
	return v;
}

internal inline vec3_t 
operator *(vec3_t a, r32 s) 
{
	vec3_t v = {};
    
	v[0] = a[0] * s;
	v[1] = a[1] * s;
	v[2] = a[2] * s;
    
	return v;
}

internal inline vec3_t 
operator *(r32 s, vec3_t a) 
{
	vec3_t v = {};
    
	v[0] = a[0] * s;
	v[1] = a[1] * s;
	v[2] = a[2] * s;
    
	return v;
}

internal inline r32 
operator *(vec3_t a, vec3_t b) 
{
r32 result = {};
result = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];

return result;
}

internal inline r32 
Dot3(vec3_t v1, vec3_t v2) 
{
	r32 result = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    
	return result;
}

internal inline s32 
Dot3i(vec3i_t v1, vec3i_t v2) 
{
	s32 result = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    
	return result;
}

internal inline r32 
Dot3(r32 *v1, r32 *v2) 
{
	r32 result = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    
	return result;
}

internal inline r32 
Dot3(r32 *v1, vec3_t v2) 
{
	r32 result = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    
	return result;
}

internal inline r32 
Dot3(vec3_t v1, r32 *v2) 
{
	r32 result = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    
	return result;
}

internal inline vec3_t
Lerp(vec3_t a, vec3_t b, r32 t)
{
vec3_t result = {};
result[0] = (1.0f - t)*a[0] + t*b[0];
result[1] = (1.0f - t)*a[1] + t*b[1];
result[2] = (1.0f - t)*a[2] + t*b[2];

return result;
}

internal inline vec3_t
Vec3Norm(vec3_t v) 
{
	vec3_t n = {};
	r32 len; 
r32 ilen;
    
	len = Dot3(v, v);
	len = (r32)sqrt(len);
    
	if (len) 
{
		ilen = 1.0f / len;
		n[0] = v[0] * ilen;
		n[1] = v[1] * ilen;
		n[2] = v[2] * ilen;
	}
    
	return n;
}

internal inline vec3_t 
Cross3(vec3_t v1, vec3_t v2) 
{
	vec3_t result = {};
    
	result[0] = (v1[1]*v2[2]) - (v1[2]*v2[1]);
	result[1] = (v1[2]*v2[0]) - (v1[0]*v2[2]);
	result[2] = (v1[0]*v2[1]) - (v1[1]*v2[0]);
    
	return result;
}

internal inline vec3_t
Hadamard(vec3_t a, vec3_t b)
{
vec3_t result = {};
result[0] = a[0]*b[0];
result[1] = a[1]*b[1];
result[2] = a[2]*b[2];

return result;
}

internal inline u32 
RoundReal32ToU32(r32 value) 
{
	u32 result = (u32)(value + 0.5f);
	return result;
}

internal u32 
PackRGBA(vec4_t color) 
{
	u32 result = (RoundReal32ToU32(color.a * 255.0f) << 24 |
				  RoundReal32ToU32(color.r * 255.0f) << 16 |
				  RoundReal32ToU32(color.g * 255.0f) << 8  |
				  RoundReal32ToU32(color.b * 255.0f));
    

	return result;
}

internal vec4_t
Linear1ToSRGB(vec4_t color)
{
vec4_t result;

result.r = (r32)(sqrt(color.r));
result.g = (r32)(sqrt(color.g));
result.b = (r32)(sqrt(color.b));
result.a = color.a;

return result;
}

internal r32
ExactLinearTosRGB(r32 linear)
{
r32 result;
if(linear < 0.0f)
{
linear = 0.0f;
}
else if(linear > 1.0f)
{
linear = 1.0f;
}

result = linear*12.92f;
if(linear > 0.0031308f)
{
result = 1.055f*(r32)pow(linear, 1.0f/2.4f) - 0.055f;
}

return result;
}

internal r32
RandomUnilateral()
{
r32 result = (r32)rand() / (r32)RAND_MAX;

return result;
}

internal r32
RandomBilateral()
{
r32 result = -1.0f + 2.0f*RandomUnilateral();

return result;
}

typedef struct plane_t
{
vec3_t n;
r32 d;
u32 material_index;
} plane_t;

typedef struct material_t
{
r32 scatter;
vec3_t emit_color;
vec3_t refl_color;
} material_t;

typedef struct sphere_t
{
r32 r;
vec3_t p;
u32 material_index;
} sphere_t;

typedef struct world_t
{
u32 num_materials;
material_t *materials;
u32 num_planes;
plane_t *planes;
u32 num_spheres;
sphere_t *spheres;
} world_t;

typedef struct viewpoint_t
{
vec3_t pos;
vec3_t local_x;
vec3_t local_y;
vec3_t local_z;
} viewpoint_t;



#endif   // MATH_H