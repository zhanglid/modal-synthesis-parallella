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
e_memseg_t inpmem;
//volatile memspace *pmem = NULL;
volatile char *pmsg = NULL;
volatile float *pinp = NULL;
mstring s1;
//float inpvalue[]={ 0.1464, 0.5000, 0.8536, 1.0000, 0.8536, 0.5000, 0.1464};
unsigned row, col;
Spara sp;
memspace pmem;
int main(void)
{

	int i = 0, x = 0, j = 0;
	char name[10] = {0};

	row = e_group_config.group_row-32;
	col = e_group_config.group_col-8;

	//memory alloc
	snprintf(name, 10, "_%dmStr%d", row, col);
	if (E_OK != e_shm_attach(&mem, name)) {
		return 0;
	}
	snprintf(name, 10, "_%dmInp%d", row, col);
	if (E_OK != e_shm_attach(&inpmem, name)) {
		return 0;
	}

	volatile void* mem_addr = (volatile void*)mem.ephy_base;
	pmsg = (volatile char*)((char*)mem_addr + MSG_OFF);
	pmem.parameters = (volatile Spara*)((char*)mem_addr + PARA_OFF);
	pmem.data = (float*)((char*)mem_addr + DATA_OFF);
	pmem.syn = (volatile char*)((char*)mem_addr + SYN_OFF);
	pmem.paraUpdate = (volatile char*)((char*)mem_addr + PARAUP_OFF);
	pmem.force = (volatile char*)((char*)mem_addr + FORCE_OFF);
	pmem.reset = (volatile char*)((char*)mem_addr + RESET_OFF);
	pmem.num = (volatile unsigned *)((char*)mem_addr + NUM_OFF);
	pmem.msg = (volatile char*)((char*)mem_addr + MSG_OFF);

	pinp = (volatile float*)inpmem.ephy_base;
	do{
		memcpy((void*)&sp,(void*)(pmem.parameters), sizeof(Spara));
	}while(sp.version != pmem.parameters->version);
	memspace * test = NULL;
	//e_string_modes(&s1, &sp);
	e_string_modes_renew(&s1, &sp);
	snprintf(pmsg,128,"modes num: %d",s1.m);
	e_inp_gen(&s1, &sp);

	e_string_output(s1, &sp);
	return EXIT_SUCCESS;
}


int e_string_output(mstring string, Spara* p_para)
{
	//float oneovereta = 2.0f / p_para->L;
	//float Fs = p_para->Fs;
	float *p_PHI0, *p_PHI1, *p_PHI2, *t = NULL;
	float PHI0[MODENUM] = { 0 }, PHI1[MODENUM] = { 0 }, PHI2[MODENUM] = { 0 }, inp = 0;
	float multi;
	if (OUTPUT_FORCE)
		multi = p_para->multiF;
	if (OUTPUT_VELOCITY)
		multi = p_para->multiV;
	int i = 0, j = 0, flag = 0;//, force_count = 0;
	p_PHI0 = PHI0;
	p_PHI1 = PHI1;
	p_PHI2 = PHI2;
	pmem.syn[2] = 'a';

	while (1)
	{
		while(pmem.syn[0] == 'n'){
			float V = 0;
			float F = 0;
			j++;

			if (pmem.paraUpdate[0] == 'y'){
				e_string_modes_renew(&string, p_para);
				e_inp_gen(&string, p_para);
				while(pmem.paraUpdate[0] != 'n') pmem.paraUpdate[0] = 'n';
			}

			if(pmem.force[0] == 'y' && flag !=1 ){
				flag = 1;
				pmem.syn[2] = (pmem.syn[2]+1)%26 + 'a';
				//snprintf(pmsg,128,"multiF:%f",p_para->multiF);
			}

			if(flag == 1)
			{
				int rc;
				rc = e_getInp(&inp);
				if (rc == -1)
				{
					while(pmem.force[0] != 'x') pmem.force[0] = 'x';
					flag = 0;
					//force_count = 0;
				}
			}
			else
			{
				inp = 0;
			}
			if (pmem.reset[0] == 'y')
			{
				for (i = 0; i < string.m; i++)
				{
					p_PHI0[i] = 0;
				}
				for (i = 0; i < string.m; i++)
				{
					p_PHI1[i] = 0;
				}
				for (i = 0; i < string.m; i++)
				{
					p_PHI2[i] = 0;
				}
				do {
					pmem.reset[0] = 'n';
				} while(pmem.reset[0] != 'n');
			}
			for (i = 0; i < string.m; i++)
			{
				p_PHI0[i] = string.b1[i] * string.we[i] * inp - string.a1[i] * p_PHI1[i] - string.a2[i] * p_PHI2[i];
				if (OUTPUT_VELOCITY) V += string.wp[i] * (p_PHI0[i] - p_PHI2[i]) * string.qfac[i];
				if (OUTPUT_FORCE) F += string.wp[i] * p_PHI0[i] * string.qfac[i];
			}

			if (OUTPUT_VELOCITY){
				V = V * multi / 2.0;
				pmem.data[j-1] = V;
			}
			if (OUTPUT_FORCE) {
				F = F * multi;
			  pmem.data[j-1] = F;
			}
			t = p_PHI2;
			p_PHI2 = p_PHI1;
			p_PHI1 = p_PHI0;
			p_PHI0 = t;
			if(j == TCPSPEED){
				while(pmem.syn[0]!='y') pmem.syn[0] = 'y';
				j = 0;
			}

		}

	}
	return 1;
}
