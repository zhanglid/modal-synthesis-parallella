#ifndef THREADFORCE_H
#define THREADFORCE_H

#include "mstring.h"
void* threadForce(void *_ms);
void  ChordArrayGen(mstring* (*p_ms)[4], volatile char** p_ms_force_array, char ChordType);
extern volatile unsigned ms_index;
#endif
