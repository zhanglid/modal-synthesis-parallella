#include "inc/mymath.h"
static inline float __p_exp_ln2(const float x)
{
	const float a1 = -0.9998684f;
	const float a2 = 0.4982926f;
	const float a3 = -0.1595332f;
	const float a4 = 0.0293641f;
	float exp_x;
	exp_x = 1.f +
		a1 * x +
		a2 * x * x +
		a3 * x * x * x +
		a4 * x * x * x * x;
	return 1.f / exp_x;
}

static inline float __p_exp_pos(const float x)
{
	long int k, twok;
	float x_;
	k = x / M_LN2;
	twok = 1U << k;
	x_ = x - (float)k * M_LN2;
	return (float)twok * __p_exp_ln2(x_);
}

float _p_exp(const float x)
{
	if (x >= 0.f)
		return __p_exp_pos(x);
	else
		return 1.f / __p_exp_pos(-x);
}

float p_sqrt(const float z)
{
	float x;
	union {
		float f;
		int i;
	} j;
	float xhalf = 0.5f*z;

	j.f = z;
	j.i = 0x5f375a86 - (j.i >> 1);
	x = j.f;

	// Newton steps, repeating this increases accuracy
	x = x*(1.5f - xhalf*x*x);
	x = x*(1.5f - xhalf*x*x);
	x = x*(1.5f - xhalf*x*x);

	// x contains the inverse sqrt

	// Multiply the inverse sqrt by the input to get the sqrt
	return z * x;
}


float p_cos_f32(const float a)
{

		float val = 1;
		int k;
		float theta = a;
		//float theta = M_NORMALIZE_RADIANS(*pa);
		while (theta >= 3.1415927f) {
			theta -= 2* 3.1415926;
		}
		while (theta <= -3.1415927f) {
			theta += 2* 3.1415926;
		}
		val = 1.0f - theta*theta*0.083333333f*0.090909090f*val;
		val = 1.0f - theta*theta*0.10000000f*0.11111111f*val;
		val = 1.0f - theta*theta*0.12500000f*0.14285714f*val;
		val = 1.0f - theta*theta*0.16666667f*0.20000000f*val;
		val = 1.0f - theta*theta*0.25000000f*0.33333333f*val;
		val = 1.0f - theta*theta*0.50000000f*1.00000000f*val;

		return val;

}

float p_sin_f32(float theta)
{
		float val = 1.0f;
		//int k;

		while (theta >= 3.14159265) {
			theta -= 2 * 3.14159265;
		}
		while (theta <= -3.14159265) {
			theta += 2 * 3.14159265;
		}

		val = 1.0f - theta * theta * 0.083333333f * 0.076923077f * val;
		val = 1.0f - theta * theta * 0.1f * 0.090909091f * val;
		val = 1.0f - theta * theta * 0.125f * 0.111111111f * val;
		val = 1.0f - theta * theta * 0.166666667f * 0.142857143f * val;
		val = 1.0f - theta * theta * 0.25f * 0.2f * val;
		val = 1.0f - theta * theta * 0.5f * 0.333333333f * val;

		return theta * val;
	}

void m_sin_f32(const int m, float theta, float *dst){
	if (m <= 0) return;
	int i;
	float cos1m = p_cos_f32(theta);
	float sin1m = p_sin_f32(theta);
	dst[0] = cos1m;
	float cos_now = cos1m;
	float sin_now = sin1m;
	for (i = 1; i < m; i++) {
		float t_cos = cos_now * cos1m - sin_now * sin1m;
		sin_now = sin_now * cos1m + cos_now * sin1m;
		cos_now = t_cos;
		dst[i] = sin_now;
	}
}

void m_cos_f32(const int m, float theta, float *dst){
	if (m <= 0) return;
	int i;
	float cos1m = p_cos_f32(theta);
	float sin1m = p_sin_f32(theta);
	dst[0] = cos1m;
	float cos_now = cos1m;
	float sin_now = sin1m;
	for (i = 1; i < m; i++) {
		float t_cos = cos_now * cos1m - sin_now * sin1m;
		sin_now = sin_now * cos1m + cos_now * sin1m;
		cos_now = t_cos;
		dst[i] = cos_now;
	}
}
void mm_exp_f32(const int m, float theta_b, float theta_a, float *dst){
	float em1 = _p_exp(theta_b);
	float ema = _p_exp(theta_a);
	float em1_now = em1;
	float em2_now =  em1 * em1;
	dst[0] = ema * em1;
	int i;
	for (i = 1; i < m; i++) {
		em1_now = em1_now * em2_now * em1;
		em2_now = em2_now * em1 * em1;
		dst[i] = ema * em1_now;
	}
}
