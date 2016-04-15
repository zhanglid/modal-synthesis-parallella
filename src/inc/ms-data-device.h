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
	volatile char reset;
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
	float qfac[MODENUM];
	int m;
}mstring;
#endif
