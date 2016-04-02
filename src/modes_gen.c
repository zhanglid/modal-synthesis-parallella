/************************************************************************/
/*
	This is a final year project of sound synthesis on parallella board.
	Website: https://github.com/dzl199401/Modal-Synthesis-Parallella

	This is the function for the DEVICE side of the program.
  e_string_modes() used to initialize the string mode parameters
  e_string_modesUpdate() used to update the mode parameters, when we change
  it in real-time.
*/
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "e_lib.h"
#include "inc/modes_gen.h"
//#include "inc/e_mstring.h"
static inline float p_cos_f32(const float a)
{

		float val = 1;
		int k;
		float theta = a;
		//float theta = M_NORMALIZE_RADIANS(*pa);
		while (theta >= 3.1415927f) {
			theta -= 2* 3.1415926;
		}
		while (theta <= -3.1415927f) {
			theta += 2* 3.1415926;
		}
		val = 1.0f - theta*theta*0.083333333f*0.090909090f*val;
		val = 1.0f - theta*theta*0.10000000f*0.11111111f*val;
		val = 1.0f - theta*theta*0.12500000f*0.14285714f*val;
		val = 1.0f - theta*theta*0.16666667f*0.20000000f*val;
		val = 1.0f - theta*theta*0.25000000f*0.33333333f*val;
		val = 1.0f - theta*theta*0.50000000f*1.00000000f*val;

		return val;

}

int e_string_modes(mstring* n_string, Spara* p_para)
{
	int   m = 0, i = 0;	//counter for mode and store
	float k, alph, w, f, wwe, wwp, R;//variables for temporary store the modes parameters
	float varrho = p_para->A*p_para->rho;//the oneverall normailzarion factor
	float b1_prefix = ((p_para->T*p_para->T) / varrho);//reduce calculation
	float kpm = PI / p_para->L;
	float wpre1 = ((p_para->kappa*p_para->kappa)-(p_para->b2*p_para->b2))*kpm*kpm*kpm*kpm;
	float wpre2 = (p_para->c*p_para->c-2*p_para->b1*p_para->b2)*kpm*kpm;
	float wpre3 = p_para->b1*p_para->b1;

	n_string->wpre[0] = wpre1;
	n_string->wpre[1] = wpre2;
	n_string->wpre[2] = wpre3;

	float rpre1 = -p_para->T*p_para->b1;
	float rpre2 = -p_para->T*p_para->b2*kpm*kpm;

	n_string->rpre[0] = rpre1;
	n_string->rpre[1] = rpre2;

	float wmax = PI*p_para->Fs;
	float prewe = kpm*p_para->xe;
	float prewp = kpm*p_para->xp;

	n_string->prewe = prewe;
	n_string->prewp = prewp;

	int mm = 0;
	n_string->m = 0;

	for (m = 1; m <= MODENUM; m++)
	{
		mm = m * m;
		w = sqrt((wpre1*mm + wpre2) * mm - wpre3);

		if (w > 0 && w < wmax)
		{
			i++;
			R = exp(rpre1 + rpre2 * mm);
			n_string->R[i - 1] = R;
			n_string->w[i - 1] = w;
			n_string->we[i - 1] = sin(m * prewe);
			if (OUTPUT_VELOCITY) n_string->wp[i - 1] = sin(m * prewp);
			n_string->a1[i - 1] = -2 * R * cos(w * p_para->T);
			n_string->a2[i - 1] = R * R;
			n_string->b1[i - 1] = b1_prefix * R;
			n_string->m++;
			if (OUTPUT_FORCE)	n_string->wp[i - 1] = -(p_para->Te * m * kpm + (p_para->E) * (p_para->I) * mm * m * kpm * kpm * kpm) * cos(m * kpm * p_para->L);
			n_string->dwep[i - 1] = m * cos(m * prewe);
		if (OUTPUT_VELOCITY)	n_string->dwpp[i - 1] = m * cos(m * prewp);
			n_string->dRr1[i - 1] = R;
			n_string->dRr2[i - 1] = mm * R;
			n_string->da1R[i - 1] = -2 * cos(w * p_para->T);
			n_string->da1w[i - 1] = 2 * R * p_para->T * sin(w * p_para->T);
			n_string->dwp3[i - 1] = -1 / (2 * w);
			n_string->dwp2[i - 1] = 1 / (2 * w) * mm;
			n_string->dwp1[i - 1] = 1 / (2 * w) * mm * mm;

		if (OUTPUT_FORCE){
			n_string->dwft[i - 1] = -m * kpm * cos(m * kpm * p_para->L);
			n_string->dwfei[i - 1] = -mm * m * kpm * kpm * kpm * cos(m * kpm * p_para->L);
			n_string->dwfkpm[i - 1] = -(m * p_para->Te+3 * kpm * kpm * (p_para->E) * (p_para->I) * mm * m) * cos(m * kpm * p_para->L)+m * p_para->L * (p_para->Te * m * kpm+(p_para->E) * (p_para->I) * mm * m * kpm * kpm * kpm) * sin(m * kpm * p_para->L);
		}
		}

	}
	return 1;
}

int e_string_modesUpdate(mstring* n_string, Spara* p_para){
	int m=0;
	float t=0;
	float dEI = (p_para->E)*(p_para->I);
	float dTe = p_para->Te;
	float dkpm = PI / p_para->L;
	while(p_para->version!=pmem->parameters.version) memcpy(p_para,(void*)&(pmem->parameters),sizeof(Spara)); //read new parameters
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

	memcpy((void*)&(pmem->parameters),p_para,sizeof(Spara));
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
  return 1;
}
