#ifndef MYMATH_H
#define  MYMATH_H
#include <math.h>
static inline float __p_exp_ln2(const float x);
static inline float __p_exp_pos(const float x);
float _p_exp(const float x);
float p_sqrt(const float z);
float p_cos_f32(const float a);
float p_sin_f32(float theta);
void m_sin_f32(const int m, float theta, float *dst);
void m_cos_f32(const int m, float theta, float *dst);
void mm_exp_f32(const int m, float theta_b, float theta_a, float *dst);
#endif
