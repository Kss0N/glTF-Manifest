#ifndef LINMATH_H
#define LINMATH_H

#include <string.h>
#include <math.h>
#include <string.h>

#ifdef LINMATH_NO_INLINE
#define LINMATH_H_FUNC static
#else
#define LINMATH_H_FUNC static inline
#endif

#define LINMATH_H_DEFINE_VEC(n) \
typedef float vec##n[n]; \
LINMATH_H_FUNC float* vec##n##_add(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const a, _In_reads_(n) vec##n const b) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = a[i] + b[i]; \
	return r;\
} \
LINMATH_H_FUNC float* vec##n##_sub(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const a, _In_reads_(n) vec##n const b) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = a[i] - b[i]; \
	return r;\
} \
LINMATH_H_FUNC float* vec##n##_scale(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const v, _In_ float const s) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = v[i] * s; \
	return r;\
} \
LINMATH_H_FUNC float vec##n##_mul_inner(_In_reads_(n) vec##n const a, _In_reads_(n) vec##n const b) \
{ \
	float p = 0.f; \
	int i; \
	for(i=0; i<n; ++i) \
		p += b[i]*a[i]; \
	return p; \
} \
LINMATH_H_FUNC float vec##n##_len(_In_reads_(n) vec##n const v) \
{ \
	return sqrtf(vec##n##_mul_inner(v,v)); \
} \
LINMATH_H_FUNC float* vec##n##_norm(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const v) \
{ \
	float k = 1.f / vec##n##_len(v); \
	vec##n##_scale(r, v, k); \
	return r;\
} \
LINMATH_H_FUNC float* vec##n##_min(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const a, _In_reads_(n) vec##n const b) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = a[i]<b[i] ? a[i] : b[i]; \
	return r;\
} \
LINMATH_H_FUNC float* vec##n##_max(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const a, _In_reads_(n) vec##n const b) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = a[i]>b[i] ? a[i] : b[i]; \
	return r;\
} \
LINMATH_H_FUNC float* vec##n##_dup(_Out_writes_(n) vec##n r, _In_reads_(n) vec##n const src) \
{ \
	int i; \
	for(i=0; i<n; ++i) \
		r[i] = src[i]; \
	return r;\
} \
LINMATH_H_FUNC float vec##n##_angle(_In_reads_(n) vec##n const a, _In_reads_(n) vec##n const b) { \
	return acosf(vec##n##_mul_inner(a, b) / (vec##n##_len(a) * vec##n##_len(b))); \
}

LINMATH_H_DEFINE_VEC(2)
LINMATH_H_DEFINE_VEC(3)
LINMATH_H_DEFINE_VEC(4)



typedef float quat[4];
#define quat_add vec4_add
#define quat_sub vec4_sub
#define quat_norm vec4_norm
#define quat_scale vec4_scale
#define quat_mul_inner vec4_mul_inner


LINMATH_H_FUNC float* vec3_mul_cross(_Out_writes_(3) vec3 r, _In_reads_(3) vec3 const a, _In_reads_(3) vec3 const b)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
	return r;
}

LINMATH_H_FUNC float* vec3_reflect	(_Out_writes_(3) vec3 r, _In_reads_(3) vec3 const v, _In_reads_(3) vec3 const n)
{
	float p = 2.f * vec3_mul_inner(v, n);
	int i;
	for (i = 0; i < 3; ++i)
		r[i] = v[i] - p * n[i];
	return r;
}

LINMATH_H_FUNC float* vec4_mul_cross(_Out_writes_(4) vec4 r, _In_reads_(4) vec4 const a, _In_reads_(4) vec4 const b)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
	r[3] = 1.f;
	return r;
}

LINMATH_H_FUNC float* vec4_reflect	(_Out_writes_(4) vec4 r, _In_reads_(4) vec4 const v, _In_reads_(4) vec4 const n)
{
	float p = 2.f * vec4_mul_inner(v, n);
	int i;
	for (i = 0; i < 4; ++i)
		r[i] = v[i] - p * n[i];
	return r;
}


typedef vec3 mat3[3];

LINMATH_H_FUNC vec3 * mat3_identity				(_Out_writes_(3) mat3 M)
{
	int i, j;
	for (i = 0; i < 3; ++i)
		for (j = 0; j < 3; ++j)
			M[i][j] = i == j ? 1.f : 0.f;
	return M;
}
LINMATH_H_FUNC vec3 * mat3_dup					(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const N)
{
	int i;
	for (i = 0; i < 3; ++i)
		vec3_dup(M[i], N[i]);
	return M;
}
LINMATH_H_FUNC float* mat3_row					(_Out_writes_(3) vec3 r, _In_reads_(3) mat3 const M, _In_ unsigned int i)
{
	int k;
	for (k = 0; k < 3; ++k)
		r[k] = M[k][i];
	return r;
}
LINMATH_H_FUNC float* mat3_col					(_Out_writes_(3) vec3 r, _In_reads_(3) mat3 const M, _In_ unsigned int i)
{
	int k;
	for (k = 0; k < 3; ++k)
		r[k] = M[i][k];
	return r;
}
LINMATH_H_FUNC vec3 * mat3_transpose			(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const N)
{
	mat3 tmp;

	int i, j;
	for (j = 0; j < 3; ++j)
		for (i = 0; i < 3; ++i)
			tmp[i][j] = N[j][i];
	return mat3_dup(M, tmp);
}
LINMATH_H_FUNC vec3 * mat3_add					(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const a, _In_reads_(3) mat3 const b)
{
	int i;
	for (i = 0; i < 3; ++i)
		vec3_add(M[i], a[i], b[i]);
	return M;
}
LINMATH_H_FUNC vec3 * mat3_sub					(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const a, _In_reads_(3) mat3 const b)
{
	int i;
	for (i = 0; i < 3; ++i)
		vec3_sub(M[i], a[i], b[i]);
	return M;
}
LINMATH_H_FUNC vec3 * mat3_scale				(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const a, _In_ float k)
{
	int i;
	for (i = 0; i < 3; ++i)
		vec3_scale(M[i], a[i], k);
	return M;
}
LINMATH_H_FUNC vec3 * mat3_scale_aniso			(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const a, _In_ float x, _In_ float y, _In_ float z)
{
	vec3_scale(M[0], a[0], x);
	vec3_scale(M[1], a[1], y);
	vec3_scale(M[2], a[2], z);
	vec3_dup(M[3], a[3]);
	return M;
}
LINMATH_H_FUNC vec3 * mat3_mul					(_Out_writes_(3) mat3 M, _In_reads_(3) mat3 const a, _In_reads_(3) mat3 const b)
{
	mat3 temp;
	int k, r, c;
	for (c = 0; c < 3; ++c) for (r = 0; r < 3; ++r) {
		temp[c][r] = 0.f;
		for (k = 0; k < 3; ++k)
			temp[c][r] += a[k][r] * b[c][k];
	}
	return mat3_dup(M, temp);
}
LINMATH_H_FUNC float* mat3_mul_vec3				(_Out_writes_(3) vec3 r, _In_reads_(3) mat3 const M, _In_reads_(3) vec3 const v)
{
	int i, j;
	for (j = 0; j < 3; ++j) {
		r[j] = 0.f;
		for (i = 0; i < 3; ++i)
			r[j] += M[i][j] * v[i];
	}
	return r;
}
LINMATH_H_FUNC vec3 * mat3_translate			(_Out_writes_(3) mat3 T, _In_ float x, _In_ float y)
{
	mat3_identity(T);
	T[3][0] = x;
	T[3][1] = y;
	return T;
}
LINMATH_H_FUNC vec3 * mat3_translate_in_place	(_Out_writes_(3) mat3 M, _In_ float x, _In_ float y)
{
	vec3 t = { x, y, 0 };
	vec3 r;
	int i;
	for (i = 0; i < 3; ++i) {
		mat3_row(r, M, i);
		M[3][i] += vec3_mul_inner(r, t);
	}
	return M;
}
LINMATH_H_FUNC vec3 * mat3_from_vec3_mul_outer	(_Out_writes_(3) mat3 M, _In_reads_(3) vec3 const a, _In_reads_(3) vec3 const b)
{
	int i, j;
	for (i = 0; i < 2; ++i) for (j = 0; j < 2; ++j)
		M[i][j] = i < 3 && j < 3 ? a[i] * b[j] : 0.f;
	return M;
}
LINMATH_H_FUNC vec3 * mat3_rotate				(_Out_writes_(3) mat3 R, _In_reads_(3) mat3 const M, _In_ float x, _In_ float y, _In_ float z, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	vec3 u = { x, y, z };

	if (vec3_len(u) > 1e-4) {
		//TODO
		return NULL;
	}
	else {
		return mat3_dup(R, M);
	}
}
LINMATH_H_FUNC vec3 * mat3_rotate_X				(_Out_writes_(3) mat3 Q, _In_reads_(3) mat3 const M, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat3 R = {
		{1.f, 0.f, 0.f},
		{0.f,   c,   s},
		{0.f,  -s,   c},
	};
	return mat3_mul(Q, M, R);
}
LINMATH_H_FUNC vec3 * mat3_rotate_Y				(_Out_writes_(3) mat3 Q, _In_reads_(3) mat3 const M, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat3 R = {
		{   c, 0.f,  -s},
		{ 0.f, 1.f, 0.f},
		{   s, 0.f,   c},
	};
	return mat3_mul(Q, M, R);
}
LINMATH_H_FUNC vec3 * mat3_rotate_Z				(_Out_writes_(3) mat3 Q, _In_reads_(3) mat3 const M, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat3 R = {
		{   c,   s, 0.f},
		{  -s,   c, 0.f},
		{ 0.f, 0.f, 1.f},
	};
	return mat3_mul(Q, M, R);
}
LINMATH_H_FUNC vec3 * mat3_invert				(_Out_writes_(3) mat3 T, _In_reads_(3) mat3 const M)
{
	float det;
	float 
		a = M[0][0],
		b = M[0][1], 
		c = M[0][2],
		d = M[1][0], 
		e = M[1][1],
		f = M[1][2],
		g = M[2][0], 
		h = M[2][1], 
		i = M[2][2];

	T[0][0] = e * i - f * h;
	T[0][1] = -(b * i - h * c);
	T[0][2] = b * f - e * c;
	T[1][0] = -(d * i - g * f);
	T[1][1] = a * i - c * g;
	T[1][2] = -(a * f - d * c);
	T[2][0] = d * h - g * e;
	T[2][1] = -(a * h - g * b);
	T[2][2] = a * e - b * d;

	det = 1.0f / (a * T[0][0] + b * T[1][0] + c * T[2][0]);

	return mat3_scale(T, T, det);
}
LINMATH_H_FUNC vec3 * mat3_orthonormalize		(_Out_writes_(3) mat3 R, _In_reads_(3) mat3 const M)
{
	mat3_dup(R, M);
	float s = 1.f;
	vec3 h;

	vec3_norm(R[2], R[2]);

	s = vec3_mul_inner(R[1], R[2]);
	vec3_scale(h, R[2], s);
	vec3_sub(R[1], R[1], h);
	vec3_norm(R[1], R[1]);

	s = vec3_mul_inner(R[0], R[2]);
	vec3_scale(h, R[2], s);
	vec3_sub(R[0], R[0], h);

	s = vec3_mul_inner(R[0], R[1]);
	vec3_scale(h, R[1], s);
	vec3_sub(R[0], R[0], h);
	vec3_norm(R[0], R[0]);

	return R;
}


;
typedef vec4 mat4x4[4];
typedef mat4x4 mat4;

LINMATH_H_FUNC vec4 * mat4x4_identity			(_Out_writes_(4) mat4 M)
{
	int i, j;
	for (i = 0; i < 4; ++i)
		for (j = 0; j < 4; ++j)
			M[i][j] = i == j ? 1.f : 0.f;
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_dup				(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const N)
{
	int i;
	for (i = 0; i < 4; ++i)
		vec4_dup(M[i], N[i]);
	return M;
}
LINMATH_H_FUNC float* mat4x4_row				(_Out_writes_(4) vec4 r, _In_reads_(4) mat4 const M, _In_ unsigned int i)
{
	int k;
	for (k = 0; k < 4; ++k)
		r[k] = M[k][i];
	return r;
}
LINMATH_H_FUNC float* mat4x4_col				(_Out_writes_(4) vec4 r, _In_reads_(4) mat4 const M, _In_ unsigned int i)
{
	int k;
	for (k = 0; k < 4; ++k)
		r[k] = M[i][k];
	return r;
}
LINMATH_H_FUNC vec4 * mat4x4_transpose			(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const N)
{
	mat4x4 tmp;

	int i, j;
	for (j = 0; j < 4; ++j)
		for (i = 0; i < 4; ++i)
			tmp[i][j] = N[j][i];
	return mat4x4_dup(M, tmp);
}
LINMATH_H_FUNC vec4 * mat4x4_add				(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const a, _In_reads_(4) mat4 const b)
{
	int i;
	for (i = 0; i < 4; ++i)
		vec4_add(M[i], a[i], b[i]);
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_sub				(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const a, _In_reads_(4) mat4 const b)
{
	int i;
	for (i = 0; i < 4; ++i)
		vec4_sub(M[i], a[i], b[i]);
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_scale				(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const a, _In_ float k)
{
	int i;
	for (i = 0; i < 4; ++i)
		vec4_scale(M[i], a[i], k);
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_scale_aniso		(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const a, _In_ float x, _In_ float y, _In_ float z)
{
	vec4_scale(M[0], a[0], x);
	vec4_scale(M[1], a[1], y);
	vec4_scale(M[2], a[2], z);
	vec4_dup(M[3], a[3]);
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_mul				(_Out_writes_(4) mat4 M, _In_reads_(4) mat4 const a, _In_reads_(4) mat4 const b)
{
	mat4x4 temp;
	int k, r, c;
	for (c = 0; c < 4; ++c) for (r = 0; r < 4; ++r) {
		temp[c][r] = 0.f;
		for (k = 0; k < 4; ++k)
			temp[c][r] += a[k][r] * b[c][k];
	}
	return mat4x4_dup(M, temp);
}
LINMATH_H_FUNC float* mat4x4_mul_vec4			(_Out_writes_(4) vec4 r, _In_reads_(4) mat4 const M, _In_reads_(4) vec4 const v)
{
	int i, j;
	for (j = 0; j < 4; ++j) {
		r[j] = 0.f;
		for (i = 0; i < 4; ++i)
			r[j] += M[i][j] * v[i];
	}
	return r;
}
LINMATH_H_FUNC vec4 * mat4x4_translate			(_Out_writes_(4) mat4 T, _In_ float x, _In_ float y, _In_ float z)
{
	mat4x4_identity(T);
	T[3][0] = x;
	T[3][1] = y;
	T[3][2] = z;
	return T;
}
LINMATH_H_FUNC vec4 * mat4x4_translate_in_place	(_Out_writes_(4) mat4 M, _In_ float x, _In_ float y, _In_ float z)
{
	vec4 t = { x, y, z, 0 };
	vec4 r;
	int i;
	for (i = 0; i < 4; ++i) {
		mat4x4_row(r, M, i);
		M[3][i] += vec4_mul_inner(r, t);
	}
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_from_vec3_mul_outer(_Out_writes_(4) mat4 M, _In_reads_(3) vec3 const a, _In_reads_(3) vec3 const b)
{
	int i, j;
	for (i = 0; i < 4; ++i) for (j = 0; j < 4; ++j)
		M[i][j] = i < 3 && j < 3 ? a[i] * b[j] : 0.f;
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_rotate				(_Out_writes_(4) mat4 R, _In_reads_(4) mat4 const M, _In_ float x, _In_ float y, _In_ float z, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	vec3 u = { x, y, z };

	if (vec3_len(u) > 1e-4) {
		vec3_norm(u, u);
		mat4x4 T;
		mat4x4_from_vec3_mul_outer(T, u, u);

		mat4x4 S = {
			{    0,  u[2], -u[1], 0},
			{-u[2],     0,  u[0], 0},
			{ u[1], -u[0],     0, 0},
			{    0,     0,     0, 0}
		};
		mat4x4_scale(S, S, s);

		mat4x4 C;
		mat4x4_identity(C);
		mat4x4_sub(C, C, T);

		mat4x4_scale(C, C, c);

		mat4x4_add(T, T, C);
		mat4x4_add(T, T, S);

		T[3][3] = 1.f;
		return mat4x4_mul(R, M, T);
	}
	else {
		return mat4x4_dup(R, M);
	}
}
LINMATH_H_FUNC vec4 * mat4x4_rotate_X			(_Out_writes_(4) mat4 Q, _In_reads_(4) mat4 const M, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{1.f, 0.f, 0.f, 0.f},
		{0.f,   c,   s, 0.f},
		{0.f,  -s,   c, 0.f},
		{0.f, 0.f, 0.f, 1.f}
	};
	return mat4x4_mul(Q, M, R);
}
LINMATH_H_FUNC vec4 * mat4x4_rotate_Y			(_Out_writes_(4) mat4 Q, _In_reads_(4) mat4 const M, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{   c, 0.f,  -s, 0.f},
		{ 0.f, 1.f, 0.f, 0.f},
		{   s, 0.f,   c, 0.f},
		{ 0.f, 0.f, 0.f, 1.f}
	};
	return mat4x4_mul(Q, M, R);
}
LINMATH_H_FUNC vec4 * mat4x4_rotate_Z			(_Out_writes_(4) mat4 Q, _In_reads_(4) mat4 const M, _In_ float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{   c,   s, 0.f, 0.f},
		{  -s,   c, 0.f, 0.f},
		{ 0.f, 0.f, 1.f, 0.f},
		{ 0.f, 0.f, 0.f, 1.f}
	};
	return mat4x4_mul(Q, M, R);
}
LINMATH_H_FUNC vec4 * mat4x4_invert				(_Out_writes_(4) mat4 T, _In_reads_(4) mat4 const M)
{
	float s[6];
	float c[6];
	s[0] = M[0][0] * M[1][1] - M[1][0] * M[0][1];
	s[1] = M[0][0] * M[1][2] - M[1][0] * M[0][2];
	s[2] = M[0][0] * M[1][3] - M[1][0] * M[0][3];
	s[3] = M[0][1] * M[1][2] - M[1][1] * M[0][2];
	s[4] = M[0][1] * M[1][3] - M[1][1] * M[0][3];
	s[5] = M[0][2] * M[1][3] - M[1][2] * M[0][3];

	c[0] = M[2][0] * M[3][1] - M[3][0] * M[2][1];
	c[1] = M[2][0] * M[3][2] - M[3][0] * M[2][2];
	c[2] = M[2][0] * M[3][3] - M[3][0] * M[2][3];
	c[3] = M[2][1] * M[3][2] - M[3][1] * M[2][2];
	c[4] = M[2][1] * M[3][3] - M[3][1] * M[2][3];
	c[5] = M[2][2] * M[3][3] - M[3][2] * M[2][3];

	/* Assumes it is invertible */
	float idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

	T[0][0] = (M[1][1] * c[5] - M[1][2] * c[4] + M[1][3] * c[3]) * idet;
	T[0][1] = (-M[0][1] * c[5] + M[0][2] * c[4] - M[0][3] * c[3]) * idet;
	T[0][2] = (M[3][1] * s[5] - M[3][2] * s[4] + M[3][3] * s[3]) * idet;
	T[0][3] = (-M[2][1] * s[5] + M[2][2] * s[4] - M[2][3] * s[3]) * idet;

	T[1][0] = (-M[1][0] * c[5] + M[1][2] * c[2] - M[1][3] * c[1]) * idet;
	T[1][1] = (M[0][0] * c[5] - M[0][2] * c[2] + M[0][3] * c[1]) * idet;
	T[1][2] = (-M[3][0] * s[5] + M[3][2] * s[2] - M[3][3] * s[1]) * idet;
	T[1][3] = (M[2][0] * s[5] - M[2][2] * s[2] + M[2][3] * s[1]) * idet;

	T[2][0] = (M[1][0] * c[4] - M[1][1] * c[2] + M[1][3] * c[0]) * idet;
	T[2][1] = (-M[0][0] * c[4] + M[0][1] * c[2] - M[0][3] * c[0]) * idet;
	T[2][2] = (M[3][0] * s[4] - M[3][1] * s[2] + M[3][3] * s[0]) * idet;
	T[2][3] = (-M[2][0] * s[4] + M[2][1] * s[2] - M[2][3] * s[0]) * idet;

	T[3][0] = (-M[1][0] * c[3] + M[1][1] * c[1] - M[1][2] * c[0]) * idet;
	T[3][1] = (M[0][0] * c[3] - M[0][1] * c[1] + M[0][2] * c[0]) * idet;
	T[3][2] = (-M[3][0] * s[3] + M[3][1] * s[1] - M[3][2] * s[0]) * idet;
	T[3][3] = (M[2][0] * s[3] - M[2][1] * s[1] + M[2][2] * s[0]) * idet;

	return T;
}
LINMATH_H_FUNC vec4 * mat4x4_orthonormalize		(_Out_writes_(4) mat4 R, _In_reads_(4) mat4 const M)
{
	mat4x4_dup(R, M);
	float s = 1.f;
	vec3 h;

	vec3_norm(R[2], R[2]);

	s = vec3_mul_inner(R[1], R[2]);
	vec3_scale(h, R[2], s);
	vec3_sub(R[1], R[1], h);
	vec3_norm(R[1], R[1]);

	s = vec3_mul_inner(R[0], R[2]);
	vec3_scale(h, R[2], s);
	vec3_sub(R[0], R[0], h);

	s = vec3_mul_inner(R[0], R[1]);
	vec3_scale(h, R[1], s);
	vec3_sub(R[0], R[0], h);
	vec3_norm(R[0], R[0]);

	return R;
}
LINMATH_H_FUNC vec4 * mat4x4_frustum			(_Out_writes_(4) mat4 M, _In_ float l, _In_ float r, _In_ float b, _In_ float t, _In_ float n, _In_ float f)
{
	M[0][0] = 2.f * n / (r - l);
	M[0][1] = M[0][2] = M[0][3] = 0.f;

	M[1][1] = 2.f * n / (t - b);
	M[1][0] = M[1][2] = M[1][3] = 0.f;

	M[2][0] = (r + l) / (r - l);
	M[2][1] = (t + b) / (t - b);
	M[2][2] = -(f + n) / (f - n);
	M[2][3] = -1.f;

	M[3][2] = -2.f * (f * n) / (f - n);
	M[3][0] = M[3][1] = M[3][3] = 0.f;

	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_ortho				(_Out_writes_(4) mat4 M, _In_ float l, _In_ float r, _In_ float b, _In_ float t, _In_ float n, _In_ float f)
{
	M[0][0] = 2.f / (r - l);
	M[0][1] = M[0][2] = M[0][3] = 0.f;

	M[1][1] = 2.f / (t - b);
	M[1][0] = M[1][2] = M[1][3] = 0.f;

	M[2][2] = -2.f / (f - n);
	M[2][0] = M[2][1] = M[2][3] = 0.f;

	M[3][0] = -(r + l) / (r - l);
	M[3][1] = -(t + b) / (t - b);
	M[3][2] = -(f + n) / (f - n);
	M[3][3] = 1.f;
	return M;
}
LINMATH_H_FUNC vec4 * mat4x4_perspective		(_Out_writes_(4) mat4 m, _In_ float y_fov, _In_ float aspect, _In_ float n, _In_ float f)
{
	/* NOTE: Degrees are an unhandy unit to work with.
	 * linmath.h uses radians for everything! */
	float const a = 1.f / tanf(y_fov / 2.f);

	m[0][0] = a / aspect;
	m[0][1] = 0.f;
	m[0][2] = 0.f;
	m[0][3] = 0.f;

	m[1][0] = 0.f;
	m[1][1] = a;
	m[1][2] = 0.f;
	m[1][3] = 0.f;

	m[2][0] = 0.f;
	m[2][1] = 0.f;
	m[2][2] = -((f + n) / (f - n));
	m[2][3] = -1.f;

	m[3][0] = 0.f;
	m[3][1] = 0.f;
	m[3][2] = -((2.f * f * n) / (f - n));
	m[3][3] = 0.f;

	return m;
}
LINMATH_H_FUNC vec4 * mat4x4_look_at			(_Out_writes_(4) mat4 m, _In_reads_(3) vec3 const eye, _In_reads_(3) vec3 const center, _In_reads_(3) vec3 const up)
{
	/* Adapted from Android's OpenGL Matrix.java.                        */
	/* See the OpenGL GLUT documentation for gluLookAt for a description */
	/* of the algorithm. We implement it in a straightforward way:       */

	/* TODO: The negation of of can be spared by swapping the order of
	 *       operands in the following cross products in the right way. */
	vec3 f;
	vec3_sub(f, center, eye);
	vec3_norm(f, f);

	vec3 s;
	vec3_mul_cross(s, f, up);
	vec3_norm(s, s);

	vec3 t;
	vec3_mul_cross(t, s, f);

	m[0][0] = s[0];
	m[0][1] = t[0];
	m[0][2] = -f[0];
	m[0][3] = 0.f;

	m[1][0] = s[1];
	m[1][1] = t[1];
	m[1][2] = -f[1];
	m[1][3] = 0.f;

	m[2][0] = s[2];
	m[2][1] = t[2];
	m[2][2] = -f[2];
	m[2][3] = 0.f;

	m[3][0] = 0.f;
	m[3][1] = 0.f;
	m[3][2] = 0.f;
	m[3][3] = 1.f;

	return mat4x4_translate_in_place(m, -eye[0], -eye[1], -eye[2]);
}

LINMATH_H_FUNC float* quat_identity				(_Out_writes_(4) quat q)
{
	q[0] = q[1] = q[2] = 0.f;
	q[3] = 1.f;
	return q;
}
LINMATH_H_FUNC float* quat_mul					(_Out_writes_(4) quat r, _In_reads_(4) quat const p, _In_reads_(4) quat const q)
{
	vec3 w, tmp;

	vec3_mul_cross(tmp, p, q);
	vec3_scale(w, p, q[3]);
	vec3_add(tmp, tmp, w);
	vec3_scale(w, q, p[3]);
	vec3_add(tmp, tmp, w);

	vec3_dup(r, tmp);
	r[3] = p[3] * q[3] - vec3_mul_inner(p, q);
	return r;
}
LINMATH_H_FUNC float* quat_conj					(_Out_writes_(4) quat r, _In_reads_(4) quat const q)
{
	int i;
	for (i = 0; i < 3; ++i)
		r[i] = -q[i];
	r[3] = q[3];
	return r;
}
LINMATH_H_FUNC float* quat_rotate				(_Out_writes_(4) quat r, _In_ float angle, _In_reads_(3) vec3 const axis) {
	vec3 axis_norm;
	vec3_norm(axis_norm, axis);
	float s = sinf(angle / 2);
	float c = cosf(angle / 2);
	vec3_scale(r, axis_norm, s);
	r[3] = c;
	return r;
}
LINMATH_H_FUNC float* quat_mul_vec3				(_Out_writes_(4) vec3 r, _In_reads_(4) quat const q, _In_reads_(3) vec3 const v)
{
	/*
	 * Method by Fabian 'ryg' Giessen (of Farbrausch)
	t = 2 * cross(q.xyz, v)
	v' = v + q.w * t + cross(q.xyz, t)
	 */
	vec3 t;
	vec3 q_xyz = { q[0], q[1], q[2] };
	vec3 u = { q[0], q[1], q[2] };

	vec3_mul_cross(t, q_xyz, v);
	vec3_scale(t, t, 2);

	vec3_mul_cross(u, q_xyz, t);
	vec3_scale(t, t, q[3]);

	vec3_add(r, v, t);
	return vec3_add(r, r, u);
}
LINMATH_H_FUNC vec4 * mat4x4_from_quat			(_Out_writes_(4) mat4 M, _In_reads_(4) quat const q)
{
	float a = q[3];
	float b = q[0];
	float c = q[1];
	float d = q[2];
	float a2 = a * a;
	float b2 = b * b;
	float c2 = c * c;
	float d2 = d * d;

	M[0][0] = a2 + b2 - c2 - d2;
	M[0][1] = 2.f * (b * c + a * d);
	M[0][2] = 2.f * (b * d - a * c);
	M[0][3] = 0.f;

	M[1][0] = 2 * (b * c - a * d);
	M[1][1] = a2 - b2 + c2 - d2;
	M[1][2] = 2.f * (c * d + a * b);
	M[1][3] = 0.f;

	M[2][0] = 2.f * (b * d + a * c);
	M[2][1] = 2.f * (c * d - a * b);
	M[2][2] = a2 - b2 - c2 + d2;
	M[2][3] = 0.f;

	M[3][0] = M[3][1] = M[3][2] = 0.f;
	M[3][3] = 1.f;

	return M;
}
LINMATH_H_FUNC vec4 * mat4x4o_mul_quat			(_Out_writes_(4) mat4 R, _In_reads_(4) mat4 const M, _In_reads_(4) quat const q)
{
	/*  XXX: The way this is written only works for orthogonal matrices. */
	/* TODO: Take care of non-orthogonal case. */
	quat_mul_vec3(R[0], q, M[0]);
	quat_mul_vec3(R[1], q, M[1]);
	quat_mul_vec3(R[2], q, M[2]);

	R[3][0] = R[3][1] = R[3][2] = 0.f;
	R[0][3] = M[0][3];
	R[1][3] = M[1][3];
	R[2][3] = M[2][3];
	R[3][3] = M[3][3];  // typically 1.0, but here we make it general

	return R;
}
LINMATH_H_FUNC float* quat_from_mat4x4			(_Out_writes_(4) quat q, _In_reads_(4) mat4 const M)
{
	float r = 0.f;
	int i;

	int perm[] = { 0, 1, 2, 0, 1 };
	int* p = perm;

	for (i = 0; i < 3; i++) {
		float m = M[i][i];
		if (m < r)
			continue;
		m = r;
		p = &perm[i];
	}

	r = sqrtf(1.f + M[p[0]][p[0]] - M[p[1]][p[1]] - M[p[2]][p[2]]);

	if (r < 1e-6) {
		q[0] = 1.f;
		q[1] = q[2] = q[3] = 0.f;
		return (float*) q;
	}

	q[0] = r / 2.f;
	q[1] = (M[p[0]][p[1]] - M[p[1]][p[0]]) / (2.f * r);
	q[2] = (M[p[2]][p[0]] - M[p[0]][p[2]]) / (2.f * r);
	q[3] = (M[p[2]][p[1]] - M[p[1]][p[2]]) / (2.f * r);

	return (float*)q;
}
LINMATH_H_FUNC vec4 * mat4x4_arcball			(_Out_writes_(4) mat4 R, _In_reads_(4) mat4 const M, _In_reads_(2) vec2 const _a, _In_reads_(2) vec2 const _b, _In_ float s)
{
	vec2 a; memcpy(a, _a, sizeof(a));
	vec2 b; memcpy(b, _b, sizeof(b));

	float z_a = 0.;
	float z_b = 0.;

	if (vec2_len(a) < 1.f) {
		z_a = sqrtf(1.f - vec2_mul_inner(a, a));
	}
	else {
		vec2_norm(a, a);
	}

	if (vec2_len(b) < 1.f) {
		z_b = sqrtf(1.f - vec2_mul_inner(b, b));
	}
	else {
		vec2_norm(b, b);
	}

	vec3 a_ = { a[0], a[1], z_a };
	vec3 b_ = { b[0], b[1], z_b };

	vec3 c_;
	vec3_mul_cross(c_, a_, b_);

	float const angle = acosf(vec3_mul_inner(a_, b_)) * s;
	return mat4x4_rotate(R, M, c_[0], c_[1], c_[2], angle);
}
#endif