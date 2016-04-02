#include <iostream>
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include "inc/threadForce.h"
void* threadForce(void *_ms){
	mstring* (*p_ms)[4] = (mstring* (*)[4])_ms;
	volatile memspace *pmem = (volatile memspace *)(p_ms[0][0]->pmem);
	volatile char* p_ms_force = &(pmem->force);
	char ch;
	while(1){
		ch = getch();
		if (ch == 'f'){
			while(*p_ms_force != 'y') p_ms_force[0] = 'y';
		}
		if(ch == 'q'){
			ms_index = (ms_index+1)%16;
			pmem = (volatile memspace *) p_ms[ms_index/4][ms_index%4]->pmem;
			p_ms_force = &(pmem->force);
		}
		if(ch == 'p'){
			float t = pmem->parameters.L;
			char v = pmem->parameters.version;
			v++;
			v = 'a' + (v - 'a') % 26;
			t *= 0.99;
			while(pmem->parameters.L != t) pmem->parameters.L = t;
			while(pmem->parameters.version != v) pmem->parameters.version = v;
			while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
		}

		if (ch == 'n'){
			float t = pmem->parameters.b1;
			char v = pmem->parameters.version;
			v++;

			v = 'a' + ( v - 'a' ) % 26;
			t *= 0.99;
			while(pmem->parameters.b1 != t) pmem->parameters.b1 = t;
			while(pmem->parameters.version != v) pmem->parameters.version = v;
			while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
		}
		if (ch == 'm'){
			float t = pmem->parameters.b2;
			char v = pmem->parameters.version;
			v++;
			v = 'a' + (v-'a') % 26;
			t *= 0.99;
			while(pmem->parameters.b2 != t) pmem->parameters.b2 = t;
			while(pmem->parameters.version != v) pmem->parameters.version = v;
			while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
		}
		usleep(1000);
	}
}
