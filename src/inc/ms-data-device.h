#ifndef MSDATADEV_H
#define MSDATADEV_H
#include "ms-spara.h"
#include "ms-const.h"
typedef struct _memspace{
	volatile Spara parameters;
  volatile float data[TCPSPEED];
  volatile char syn[3];
  volatile char paraUpdate;
  volatile char force;
  volatile unsigned num;
  volatile char msg[128];
} memspace;

typedef struct
{
	float we[MODENUM]; //excitation point
	float wp[MODENUM]; //pick-up point
	float b1[MODENUM];
	float a1[MODENUM];
	float a2[MODENUM];
	float w[MODENUM];
	float R[MODENUM];
	float alph[MODENUM];
	float dwep[MODENUM];

	float dwpp[MODENUM];

	float dRr1[MODENUM];
	float dRr2[MODENUM];

	float da1R[MODENUM];
	float da1w[MODENUM];

	float dwp1[MODENUM];
	float dwp2[MODENUM];
	float dwp3[MODENUM];

	float dwft[MODENUM];
	float dwfei[MODENUM];
	float dwfkpm[MODENUM];

	float wpre[3];
	float rpre[2];
	float prewe;
	float prewp;
	int m;
}mstring;
#endif
