#ifndef MSDATAHOST_H
#define MSDATAHOST_H
#include "ms-spara.h"
#include "ms-const.h"
#include <netinet/in.h>
typedef struct _memspace{
	volatile Spara parameters;
	volatile int none; //difference between the c and c++ to calculate the size
	volatile float data[TCPSPEED];
	volatile char syn[3];
	volatile char paraUpdate;
	volatile char force;
	volatile unsigned num;
	volatile char msg[128];
} memspace;

/*typedef struct _threadTCPdata{
	tcpclient* p_tcp;
	DATATYPE* p_buffer;
}threadTCPdata;*/
#endif
