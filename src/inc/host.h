#ifndef HOST_H
#define HOST_H
#include "ms-data-host.h"
#include "threadForce.h"
#include "threadMeasurement.h"
#include "tcpclient.h"
struct timeval tv_begin, tv_end;
struct timeval tv2_begin, tv2_end;
volatile unsigned ms_index = 0;
int sendnum = 0;
void genPara(Spara* p_para);
#endif
