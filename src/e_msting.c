// This is the DEVICE side of the program.
// The host may load this program to any eCore. When
// launched, the program queries the CoreID and output value to the shared external
// memory buffer.

#define MODENUM 100
#define OUTPUT_VELOCITY 0
#define OUTPUT_FORCE 1
#define PI 3.1416
#define TCPSPEED 512
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "e_lib.h"
#include "mymath.c"

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
	volatile    char  version;
} Spara;


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








e_memseg_t mem;
e_memseg_t msg;
volatile memspace *pmem;
volatile char *pmsg;
int time_p,time_c,time_compare;
mstring s1;
float inpvalue[]={0,5.0533,17.2530,29.4528,34.5061,29.4528,17.2530,5.0533};
unsigned row, col;
Spara sp;
int string_output(mstring string,Spara* p_para);
int string_modes(mstring* n_string, Spara* p_para);
int string_modesUpdate(mstring* n_string, Spara* p_para);

int main(void)
{

	int i = 0, x = 0, j = 0;
	char name[10]={0};

	row=e_group_config.group_row-32;
	col=e_group_config.group_col-8;

	//memory alloc
	snprintf(name,10,"_%dmStr%d",row,col);
	if (E_OK != e_shm_attach(&mem, name)) {
		return 0;
	}
	/*snprintf(name,10,"_%dmsg%d",row,col);
	if (E_OK != e_shm_attach(&msg, name)) {
		return 0;
	}*/
	//e_alloc(&mem, 0x100000*(row*4+col), 0x100000);
	//e_alloc(&msg, 0x100000*(row*4+col+16), 0x100000);
	//pmsg=(volatile char*)msg.ephy_base;//8ff00000
	//pmsg = (volatile char *) (0x8f000000 + 0x100000*(row*4+col));
	//snprintf(pmsg, 50, "Init...%d%d", row, col);
	//
	pmem = (volatile memspace*)mem.ephy_base;
	pmsg = pmem->msg;



	//pmem = (volatile memspace *)(0x8f000000 + 0x100000*(row*4+col+16));
	do{
		memcpy((void*)&sp,(void*)&(pmem->parameters),sizeof(Spara));
	}while(sp.version!=pmem->parameters.version);
	memspace* test=NULL;
	/*snprintf(pmsg, 50 ,"[0x%08x]",(test->msg));
	while (1);*/
	string_modes(&s1, &sp);
	string_output(s1,&sp);

	//end of my code

	return EXIT_SUCCESS;
}




int string_output(mstring string, Spara* p_para)
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
	pmem->syn[2]='a';
	while (1)
	{
		while(pmem->syn[0]=='n'){
			float V = 0;
			float F = 0;
			j++;

			if (pmem->paraUpdate=='y'){
				string_modesUpdate(&string, p_para);
			//	string_modes(&string, p_para);
				while(pmem->paraUpdate!='n') pmem->paraUpdate='n';
			}

			if(pmem->force=='y'){
				flag = 1;
				pmem->syn[2]=(pmem->syn[2]+1)%26+'a';
			}

			if(flag==1)
			{
				inp=inpvalue[force_count];
				force_count++;
				//snprintf(pmsg,50,"inp:%f",inp);
				//e_wait(E_CTIMER_0,3*600e6);
				if (force_count>=8)
				{
					while(pmem->force!='x') pmem->force='x';
					flag = 0;
					force_count = 0;
				}
			}
			else
			{
				inp=0;
			}

			for (i = 0; i < string.m; i++)
			{
				p_PHI0[i] = string.b1[i] * string.we[i] * inp - string.a1[i] * p_PHI1[i] - string.a2[i] * p_PHI2[i];
				if (OUTPUT_VELOCITY) V += string.wp[i] * (p_PHI0[i] - p_PHI2[i]);
				if (OUTPUT_FORCE) F += string.wp[i] * p_PHI0[i];
			}
			if (OUTPUT_VELOCITY)	V = V * multi / 2.0;
			if (OUTPUT_FORCE)	F = F * multi;
			if (OUTPUT_VELOCITY)	pmem->data[j-1]=V;
			if (OUTPUT_FORCE) pmem->data[j-1]=F;
			t = p_PHI2;
			p_PHI2 = p_PHI1;
			p_PHI1 = p_PHI0;
			p_PHI0 = t;
			//snprintf(pmsg, 128 ,"%f,%f,m:%d" ,string.b1[i],string.we[i],string.m);
			//if (flag==1) e_wait(E_CTIMER_0,3*600e6);
			if(j==TCPSPEED){
			//	snprintf(pmsg,50,"mm%dx%cx(%d,%d)",j,pmem->syn[0],row,col);
				while(pmem->syn[0]!='y') pmem->syn[0] = 'y';
				j = 0;
			}

		}

	}
	return 1;
}
int string_modes(mstring* n_string, Spara* p_para)
{
	int     m = 0, i = 0;	//counter for mode and store
	float k, alph, w, f, wwe, wwp, R;//variables for temporary store the modes parameters
	float varrho=p_para->A*p_para->rho;//the oneverall normailzarion factor
	float b1_prefix=((p_para->T*p_para->T) / varrho);//reduce calculation
	float kpm=PI / p_para->L;
	float wpre1=((p_para->kappa*p_para->kappa)-(p_para->b2*p_para->b2))*kpm*kpm*kpm*kpm;
	float wpre2=(p_para->c*p_para->c-2*p_para->b1*p_para->b2)*kpm*kpm;
	float wpre3=p_para->b1*p_para->b1;

	n_string->wpre[0]=wpre1;
	n_string->wpre[1]=wpre2;
	n_string->wpre[2]=wpre3;

	float rpre1=-p_para->T*p_para->b1;
	float rpre2=-p_para->T*p_para->b2*kpm*kpm;

	n_string->rpre[0]=rpre1;
	n_string->rpre[1]=rpre2;

	float wmax=PI*p_para->Fs;
	float prewe=kpm*p_para->xe;
	float prewp=kpm*p_para->xp;

	n_string->prewe=prewe;
	n_string->prewp=prewp;

	int mm=0;
	n_string->m = 0;

	for (m = 1; m <= MODENUM; m++)
	{
		mm=m*m;
		w=sqrt((wpre1*mm+wpre2)*mm-wpre3);

		//snprintf(pmsg, 128 ,"%f,%f,m:%d,Fs:%d" ,w,wmax,m,p_para->Fs);
		//e_wait(E_CTIMER_0,3*600e6);
		if (w>0 && w<wmax)
		{
			i++;
			R = exp(rpre1+rpre2*mm);
			n_string->R[i - 1] = R;
			n_string->w[i - 1] = w;
			n_string->we[i - 1] = sin(m*prewe);
			if (OUTPUT_VELOCITY) n_string->wp[i - 1] = sin(m*prewp);
			n_string->a1[i - 1] = -2 * R*cos(w*p_para->T);
			n_string->a2[i - 1] = R*R;
			n_string->b1[i - 1] = b1_prefix*R;
			n_string->m++;
			if (OUTPUT_FORCE)	n_string->wp[i - 1] = -(p_para->Te*m*kpm+(p_para->E)*(p_para->I)*mm*m*kpm*kpm*kpm)*cos(m*kpm*p_para->L);
/////////////////////////
			n_string->dwep[i - 1]=m*cos(m*prewe);
		if (OUTPUT_VELOCITY)	n_string->dwpp[i - 1]=m*cos(m*prewp);
			n_string->dRr1[i - 1]=R;
			n_string->dRr2[i - 1]=mm*R;
			n_string->da1R[i - 1]=-2*cos(w*p_para->T);
			n_string->da1w[i - 1]=2*R*p_para->T*sin(w*p_para->T);
	//	if (OUTPUT_VELOCITY){
			n_string->dwp3[i - 1]=-1/(2*w);
			n_string->dwp2[i - 1]=1/(2*w)*mm;
			n_string->dwp1[i - 1]=1/(2*w)*mm*mm;

		if (OUTPUT_FORCE){
			n_string->dwft[i - 1]=-m*kpm*cos(m*kpm*p_para->L);
			n_string->dwfei[i - 1]=-mm*m*kpm*kpm*kpm*cos(m*kpm*p_para->L);
			n_string->dwfkpm[i - 1]=-(m*p_para->Te+3*kpm*kpm*(p_para->E)*(p_para->I)*mm*m)*cos(m*kpm*p_para->L)+m*p_para->L*(p_para->Te*m*kpm+(p_para->E)*(p_para->I)*mm*m*kpm*kpm*kpm)*sin(m*kpm*p_para->L);
		}
//derivative parameter*
//////////////////
		}

	}
	return 1;
}

int string_modesUpdate(mstring* n_string, Spara* p_para){
	int m=0;
	float t=0;
	float dEI = (p_para->E)*(p_para->I);
	float dTe = p_para->Te;
	float dkpm = PI / p_para->L;
	while(p_para->version!=pmem->parameters.version) memcpy(p_para,&(pmem->parameters),sizeof(Spara)); //read new parameters
	p_para->xe = p_para->re*p_para->L;//update
	p_para->xp = p_para->rp*p_para->L;

	t = PI*p_para->a*p_para->a;
	while(p_para->A != t) p_para->A = t;


	t = p_para->a / 2;
	while(p_para->r != t) p_para->r = t;

	t = p_para->A*p_para->r*p_para->r;
	while(p_para->I != t) p_para->I = t;

	t = sqrt(p_para->Te / (p_para->rho*p_para->A));
	while(p_para->c != t) p_para->c = t;



	t= sqrt((p_para->E*p_para->I) / (p_para->rho*p_para->A));
	while(p_para->kappa != t) p_para->kappa = t;

	memcpy(&(pmem->parameters),p_para,sizeof(Spara));
	//calculate the increment
	dEI = (p_para->E)*(p_para->I) - dEI;
	dTe = p_para->Te - dTe;
	dkpm = PI / p_para->L - dkpm;

	float kpm=PI / p_para->L;
	float dwpre1=((p_para->kappa*p_para->kappa)-(p_para->b2*p_para->b2))*kpm*kpm*kpm*kpm-n_string->wpre[0];
	float dwpre2=(p_para->c*p_para->c-2*p_para->b1*p_para->b2)*kpm*kpm-n_string->wpre[1];
	float dwpre3=p_para->b1*p_para->b1-n_string->wpre[2];
	n_string->wpre[0]+=dwpre1;
	n_string->wpre[1]+=dwpre2;
	n_string->wpre[2]+=dwpre3;

	float drpre1=-p_para->T*p_para->b1-n_string->rpre[0];
	float drpre2=-p_para->T*p_para->b2*kpm*kpm-n_string->rpre[1];
	n_string->rpre[0]+=drpre1;
	n_string->rpre[1]+=drpre2;

	float dprewe=kpm*p_para->xe-n_string->prewe;
	float dprewp=kpm*p_para->xp-n_string->prewp;
	n_string->prewe+=dprewe;
	//n_string->prewp+=dprewp;
	if (OUTPUT_VELOCITY) {
		t=n_string->prewp+dprewp;
		while(n_string->prewp!=t) n_string->prewp=t;
	}
	float dR;
	float dw;

	float varrho=p_para->A*p_para->rho;
	float b1_prefix=((p_para->T*p_para->T) / varrho);
	for (m = 1; m <= n_string->m; m++)
	{
		//mm=m*m;
		float tmp=0;
		dw=n_string->dwp1[m-1]*dwpre1+n_string->dwp2[m-1]*dwpre2+n_string->dwp3[m-1]*dwpre3;
		t=n_string->w[m-1]+dw;
		//snprintf(pmsg,128,"dw:%f",dw);
		while(n_string->w[m-1]!=t) n_string->w[m-1]=t;
		if (t>0 && t<PI*p_para->Fs)
		{
			dR=n_string->dRr1[m-1]*drpre1+n_string->dRr2[m-1]*drpre2;

			t=n_string->R[m-1]+dR;
			while(n_string->R[m-1]!=t) n_string->R[m-1]=t;
			n_string->we[m-1] += n_string->dwep[m-1]*dprewe;
			if (OUTPUT_VELOCITY) n_string->wp[m-1] += n_string->dwpp[m-1]*dprewp;
			//n_string->a1[m-1] += n_string->da1R[m-1]*dR+n_string->da1w[m-1]*dw;
				//n_string->a1[m - 1] = -2 * n_string->R[m-1]*cos(n_string->w[m-1]*p_para->T);
				//tmp=n_string->w[m-1]*p_para->T;
			//	p_cos_f32(&tmp,&tmp);
			n_string->a1[m - 1] = -2 * n_string->R[m-1]*p_cos_f32(n_string->w[m-1]*p_para->T);
			n_string->a2[m-1] = n_string->R[m-1]*n_string->R[m-1];
			if (OUTPUT_FORCE)	n_string->wp[m-1] += n_string->dwft[m-1]*dTe + n_string->dwfei[m-1]*dEI + n_string->dwfkpm[m-1]*dkpm;
			t = b1_prefix*n_string->R[m-1];
			while(n_string->b1[m-1]!=t) n_string->b1[m-1]=t;
		}
		else
		{
			n_string->m=m;
			break;
		}

	}




}

/*int string_modes(mstring* n_string, Spara* p_para)
{
	int     m = 0, i = 0;	//counter for mode and store
	float k, alph, w, f, wwe, wwp, R;//variables for temporary store the modes parameters
	float varrho=p_para->A*p_para->rho;//the oneverall normailzarion factor
	float b1_prefix=((p_para->T*p_para->T) / varrho);//reduce calculation
	float kpm=PI / p_para->L;
	float wpre1=((p_para->kappa*p_para->kappa)-(p_para->b2*p_para->b2))*kpm*kpm*kpm*kpm;
	float wpre2=(p_para->c*p_para->c-2*p_para->b1*p_para->b2)*kpm*kpm;
	float wpre3=p_para->b1*p_para->b1;

	float rpre1=-p_para->T*p_para->b1;
	float rpre2=-p_para->T*p_para->b2*kpm*kpm;

	float wmax=PI*p_para->Fs;
	float prewe=kpm*p_para->xe;
	float prewp=kpm*p_para->xp;


	int mm=0;
	n_string->m = 0;

	for (m = 1; m <= MODENUM; m++)
	{
		mm=m*m;
		w=_p_sqrt((wpre1*mm+wpre2)*mm-wpre3);

		if (w>0 && w<wmax)
		{
			i++;
			R=_p_exp(rpre1+rpre2*mm);
			n_string->R[i - 1] = R;
			n_string->w[i - 1] = w;
			n_string->we[i - 1] = p_sin_f32(m*prewe);
			if (OUTPUT_VELOCITY) n_string->wp[i - 1] = p_sin_f32(m*prewp);
			n_string->a1[i - 1] = -2 * R*p_cos_f32(w*p_para->T);
			n_string->a2[i - 1] = R*R;
			n_string->b1[i - 1] = b1_prefix*R;
			n_string->m++;
			if (OUTPUT_FORCE)	n_string->wp[i - 1] = -(p_para->Te*m*kpm+(p_para->E)*(p_para->I)*mm*m*kpm*kpm*kpm)*p_cos_f32(m*kpm*p_para->L);
		}

	}
	return 1;
}
*/
