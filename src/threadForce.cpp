#include <iostream>
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include "inc/threadForce.h"
void* threadForce(void *_ms){
	mstring* (*p_ms)[4] = (mstring* (*)[4])_ms;
	volatile memspace *pmem = (volatile memspace *)(p_ms[0][0]->pmem);
	volatile char* p_ms_force_array[6];
	volatile char* p_ms_force = &(pmem->force);
	extern volatile bool fadeOutSet;
	extern volatile bool fadeInSet;
	ChordArrayGen(p_ms, p_ms_force_array, 'A');
/*	for (size_t i = 0; i < 6; i++) {
		volatile memspace *tmem = (volatile memspace *)(p_ms[i/4][i%4]->pmem);
		p_ms_force_array[i] = &(tmem->force);
	}*/
	char ch;
	while(1){
		ch = getch();
		switch (ch) {
			case 'f':
						{
							while(*p_ms_force != 'y') p_ms_force[0] = 'y';
						}
						break;
			case 'a':
						{
							ms_index = (ms_index+1)%16;
							pmem = (volatile memspace *) p_ms[ms_index/4][ms_index%4]->pmem;
							p_ms_force = &(pmem->force);
						}
						break;
			case 'p':
						{
							float t = pmem->parameters.L;
							char v = pmem->parameters.version;
							v++;
							v = 'a' + (v - 'a') % 26;
							t *= 0.99;
							while(pmem->parameters.L != t) pmem->parameters.L = t;
							while(pmem->parameters.version != v) pmem->parameters.version = v;
							while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
						}
						break;
			case 'n':
					{
						float t = pmem->parameters.b1;
						char v = pmem->parameters.version;
						v++;

						v = 'a' + ( v - 'a' ) % 26;
						t *= 0.99;
						while(pmem->parameters.b1 != t) pmem->parameters.b1 = t;
						while(pmem->parameters.version != v) pmem->parameters.version = v;
						while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
					}
						break;
			case 'm':
						{
							float t = pmem->parameters.b2;
							char v = pmem->parameters.version;
							v++;
							v = 'a' + (v-'a') % 26;
							t *= 0.99;
							while(pmem->parameters.b2 != t) pmem->parameters.b2 = t;
							while(pmem->parameters.version != v) pmem->parameters.version = v;
							while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
					}
						break;
			case 'q':
						{
							while((p_ms_force_array[0])[0] != 'y') (p_ms_force_array[0])[0] = 'y';
						}
						break;
			case 'w':
						{
							while((p_ms_force_array[1])[0] != 'y') (p_ms_force_array[1])[0] = 'y';
						}
						break;
			case 'e':
						{
							while((p_ms_force_array[2])[0] != 'y') (p_ms_force_array[2])[0] = 'y';
						}
						break;
			case 'r':
						{
							while((p_ms_force_array[3])[0] != 'y') (p_ms_force_array[3])[0] = 'y';
						}
						break;
			case 't':
						{
							while((p_ms_force_array[4])[0] != 'y') (p_ms_force_array[4])[0] = 'y';
							}
						break;
			case 'y':
						{
							while((p_ms_force_array[5])[0] != 'y') (p_ms_force_array[5])[0] = 'y';
						}
						break;
			case 's':
						{
								static unsigned delta_time = 10*1000; //10ms
								while((p_ms_force_array[0])[0] != 'y') (p_ms_force_array[0])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[1])[0] != 'y') (p_ms_force_array[1])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[2])[0] != 'y') (p_ms_force_array[2])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[3])[0] != 'y') (p_ms_force_array[3])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[4])[0] != 'y') (p_ms_force_array[4])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[5])[0] != 'y') (p_ms_force_array[5])[0] = 'y';
						}
						break;
			case 'd':
						{
								static unsigned delta_time = 100*1000; //100ms
								while((p_ms_force_array[0])[0] != 'y') (p_ms_force_array[0])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[1])[0] != 'y') (p_ms_force_array[1])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[2])[0] != 'y') (p_ms_force_array[2])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[3])[0] != 'y') (p_ms_force_array[3])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[4])[0] != 'y') (p_ms_force_array[4])[0] = 'y';
								usleep(delta_time);
								while((p_ms_force_array[5])[0] != 'y') (p_ms_force_array[5])[0] = 'y';
						}
						break;
			case 'z':
						{
								fadeOutSet = true;
								while (fadeOutSet == true) {
										usleep(1000);
								}
								ChordArrayGen(p_ms, p_ms_force_array, 'A');
								fadeInSet = true;
						}
						break;
			case 'x':
			      {
								fadeOutSet = true;
								while (fadeOutSet == true) {
										usleep(1000);
								}
							  ChordArrayGen(p_ms, p_ms_force_array, 'D');
								fadeInSet = true;
						}
						break;
			case 'c':
						{
								fadeOutSet = true;
								while (fadeOutSet == true) {
										usleep(1000);
								}
								ChordArrayGen(p_ms, p_ms_force_array, 'E');
								fadeInSet = true;
						}
						break;
			case ',':
						{
							float t1 = pmem->parameters.re;
							float t2 = pmem->parameters.xe;
							char v = pmem->parameters.version;
							v++;
							v = 'a' + (v - 'a') % 26;
							t1 -= 0.025f;
							if (t1 > 1)
									t1 = 1;
							if (t1 < 0)
									t1 = 0;
							t2 = t1 * pmem->parameters.L;
							while(pmem->parameters.re != t1) pmem->parameters.re = t1;
							while(pmem->parameters.xe != t2) pmem->parameters.xe = t2;
							while(pmem->parameters.version != v) pmem->parameters.version = v;
							while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
						}
						break;
				case '.':
						{
							float t1 = pmem->parameters.re;
							float t2 = pmem->parameters.xe;
							char v = pmem->parameters.version;
							v++;
							v = 'a' + (v - 'a') % 26;
							t1 += 0.025f;
							if (t1 > 1)
									t1 = 1;
							if (t1 < 0)
									t1 = 0;
							t2 = t1 * pmem->parameters.L;
							while(pmem->parameters.re != t1) pmem->parameters.re = t1;
							while(pmem->parameters.xe != t2) pmem->parameters.xe = t2;
							while(pmem->parameters.version != v) pmem->parameters.version = v;
							while(pmem->paraUpdate != 'y') pmem->paraUpdate = 'y';
						}
						break;
		}



		usleep(1000);
	}
}

void  ChordArrayGen(mstring* (*p_ms)[4], volatile char** p_ms_force_array, char ChordType){
	static const unsigned ChordIndex[3][6]={
		0, 1, 2, 3, 11, 4,					//Major A
		0, 5, 2, 6, 7, 4,						//Major D
		8, 1, 9, 10, 11, 12					//Major E
	};
	extern volatile char ChordState;
	static const unsigned *  pIndex = ChordIndex[0];
	for (size_t i = 0; i < 6; i++) {
		volatile memspace *tmem = (volatile memspace *)(p_ms[pIndex[i]/4][pIndex[i]%4]->pmem);
		do {
			tmem->reset = 'y';
		} while(tmem->reset != 'y');
	}
	switch (ChordType) {
		case 'A':
						pIndex = ChordIndex[0];
						ChordState = 'A';
						break;
		case 'D':
						pIndex = ChordIndex[1];
						ChordState = 'D';
						break;
		case 'E':
						pIndex = ChordIndex[2];
						ChordState = 'E';
						break;
		default:
						return;
	}
	for (size_t i = 0; i < 6; i++) {
		volatile memspace *tmem = (volatile memspace *)(p_ms[pIndex[i]/4][pIndex[i]%4]->pmem);
		p_ms_force_array[i] = &(tmem->force);
	}
}
