#ifndef MSPARA_H
#define MSPARA_H
typedef struct {
	volatile	unsigned Fs;
	volatile	float L;
	volatile	float a;
	volatile	float Te;
	volatile	float E;
	volatile	float rho;
	volatile	float b1;
	volatile	float b2;
	volatile	float re;
	volatile	float rp;
	volatile	float T;
	volatile	float xe;
	volatile	float xp;
	volatile	float A;
	volatile	float r;
	volatile	float I;
	volatile	float c;
	volatile	float kappa;
	volatile  float multiV;
	volatile  float multiF;
	volatile  char  version;
} Spara;
#endif
