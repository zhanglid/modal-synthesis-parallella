/************************************************************************/
/*
	This is a final year project of sound synthesis on parallella board.
	Website: https://github.com/dzl199401/Modal-Synthesis-Parallella

	This is the DEVICE side of the program.
	The host loads this program to any eCore.
	When the eCore launched, the program generate the data and output value
	to the shared external memory.
*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "inc/e_mstring.h"

e_memseg_t mem;
e_memseg_t msg;
volatile memspace *pmem;
volatile char *pmsg;
int time_p,time_c,time_compare;
mstring s1;
const float inpvalue[]={0,5.0533,17.2530,29.4528,34.5061,29.4528,17.2530,5.0533};
unsigned row, col;
Spara sp;

int main(void)
{

	int i = 0, x = 0, j = 0;
	char name[10]={0};

	row = e_group_config.group_row-32;
	col = e_group_config.group_col-8;

	//memory alloc
	snprintf(name, 10, "_%dmStr%d", row, col);
	if (E_OK != e_shm_attach(&mem, name)) {
		return 0;
	}
	pmem = (volatile memspace*)mem.ephy_base;
	pmsg = pmem->msg;

	do{
		memcpy((void*)&sp,(void*)&(pmem->parameters), sizeof(Spara));
	}while(sp.version != pmem->parameters.version);
	memspace * test = NULL;
	e_string_modes(&s1, &sp);
	e_string_output(s1, &sp);
	return EXIT_SUCCESS;
}


int e_string_output(mstring string, Spara* p_para)
{
	//float oneovereta = 2.0f / p_para->L;
	float Fs = p_para->Fs;
	float *p_PHI0, *p_PHI1, *p_PHI2, *t = NULL;
	float PHI0[MODENUM] = { 0 }, PHI1[MODENUM] = { 0 }, PHI2[MODENUM] = { 0 }, inp = 0;
	float multi;
	if (OUTPUT_FORCE)
		multi = p_para->multiF;
	if (OUTPUT_VELOCITY)
		multi = p_para->multiV;
	int i = 0, j = 0, flag = 0, force_count = 0;
	p_PHI0 = PHI0;
	p_PHI1 = PHI1;
	p_PHI2 = PHI2;
	pmem->syn[2] = 'a';
	while (1)
	{
		while(pmem->syn[0] == 'n'){
			float V = 0;
			float F = 0;
			j++;

			if (pmem->paraUpdate == 'y'){
				e_string_modesUpdate(&string, p_para);
				while(pmem->paraUpdate != 'n') pmem->paraUpdate = 'n';
			}

			if(pmem->force == 'y'){
				flag = 1;
				pmem->syn[2] = (pmem->syn[2]+1)%26 + 'a';
			}

			if(flag == 1)
			{
				inp = inpvalue[force_count];
				force_count++;
				if (force_count >= 8)
				{
					while(pmem->force != 'x') pmem->force = 'x';
					flag = 0;
					force_count = 0;
				}
			}
			else
			{
				inp = 0;
			}

			for (i = 0; i < string.m; i++)
			{
				p_PHI0[i] = string.b1[i] * string.we[i] * inp - string.a1[i] * p_PHI1[i] - string.a2[i] * p_PHI2[i];
				if (OUTPUT_VELOCITY) V += string.wp[i] * (p_PHI0[i] - p_PHI2[i]);
				if (OUTPUT_FORCE) F += string.wp[i] * p_PHI0[i];
			}
			if (OUTPUT_VELOCITY)	V = V * multi / 2.0;
			if (OUTPUT_FORCE)	F = F * multi;
			if (OUTPUT_VELOCITY)	pmem->data[j-1] = V;
			if (OUTPUT_FORCE) pmem->data[j-1] = F;
			t = p_PHI2;
			p_PHI2 = p_PHI1;
			p_PHI1 = p_PHI0;
			p_PHI0 = t;
			if(j == TCPSPEED){
				while(pmem->syn[0]!='y') pmem->syn[0] = 'y';
				j = 0;
			}

		}

	}
	return 1;
}
