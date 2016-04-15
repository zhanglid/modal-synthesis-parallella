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
#include "inc/mymath.h"
//#include "inc/e_mstring.h"


void e_inp_gen(mstring* n_string, Spara* p_para){
		size_t i;
		extern volatile float *pinp;
		static float sum = 4.4044;
		float exp_alph_t[n_string->m];
		float ampValue = (0.000008f*(p_para->c + p_para->kappa*p_para->kappa)/p_para->T)/sum;
		if (ampValue != 1){
			sum = ampValue * sum;
			for (i = 0; i < INPGUITARLENGTH; i++) {
				float t;
				t = ampValue * pinp[i];
				do {
						pinp[i] = t;
				} while(pinp[i] != t);
			}
		}
}


int e_getInp(float *inp){
		static unsigned force_count = 0;
		extern volatile float *pinp;
		*inp = pinp[force_count];
		force_count++;
		if (force_count >= INPGUITARLENGTH){
			force_count = 0;
			return -1;
		}
		return 1;
}



int e_string_modes_renew(mstring* n_string, Spara* p_para)
{
//	extern volatile volatile char *pmsg;
	while(p_para->version!=pmem->parameters.version) memcpy(p_para,(void*)&(pmem->parameters), sizeof(Spara)); //read new parameters
	int   m = 0;	//counter for mode and store
	float w, R; //variables for temporary store the modes parameters
	float b1_prefix = ((p_para->T*p_para->T) / (p_para->A*p_para->rho)); //reduce calculation
	float kpm = PI / p_para->L;
	float wpre1 = ((p_para->kappa*p_para->kappa)-(p_para->b2*p_para->b2))*kpm*kpm*kpm*kpm;
	float wpre2 = (p_para->c*p_para->c-2*p_para->b1*p_para->b2)*kpm*kpm;
	float wpre3 = p_para->b1*p_para->b1;
	float rpre1 = -p_para->T*p_para->b1;
	float rpre2 = -p_para->T*p_para->b2*kpm*kpm;
	float wmax = PI * p_para->Fs;
	float prewe = kpm*p_para->xe;
	float prewp = kpm*p_para->xp;


	int mm = 0;
	n_string->m = 0;
	if (OUTPUT_FORCE)
		m_cos_f32(MODENUM, kpm * p_para->L, n_string->wp);
	mm_exp_f32(MODENUM, rpre2, rpre1, n_string->R);
	m_sin_f32(MODENUM, prewe, n_string->we);
	//n_string->we[i - 1] = sin(m * prewe);
	mm_exp_f32(MODENUM, -(2e-5) * pow(2,QFACTOR), 0, n_string->qfac);
	for (m = 1; m <= MODENUM; m++)
	{

		mm = m * m;
		w = p_sqrt((wpre1 * mm + wpre2) * mm - wpre3);
		//n_string->qfac[m - 1] = exp(-(2e-5)*pow(2,QFACTOR)*mm);
		if (w > 0 && w < wmax)
		{

			R = n_string->R[m - 1];
			n_string->alph[m - 1] = p_para->b1 + p_para->b2 * mm * kpm * kpm;
			n_string->w[m - 1] = w;
			n_string->a1[m - 1] = -2 * R * p_cos_f32(w * p_para->T);
			n_string->a2[m - 1] = R * R;
			n_string->b1[m - 1] = b1_prefix * R;
			n_string->m++;
			if (OUTPUT_FORCE)	n_string->wp[m - 1] = -(p_para->Te * m * kpm + (p_para->E) * (p_para->I) * mm * m * kpm * kpm * kpm) * n_string->wp[m-1];
			//snprintf(pmsg, 128, "%d,q:%f,b1:%f,we:%f,a1:%f,a2:%f", m, n_string->qfac[m - 1], n_string->b1[m - 1], n_string->we[m - 1], n_string->a1[m - 1], n_string->a2[m - 1]);
			//e_wait(E_CTIMER_1, 2*600E6);
		}
		else
			break;
	}
	if (OUTPUT_VELOCITY)
		m_sin_f32(m - 1, prewp, n_string->wp);

	return 1;
}
