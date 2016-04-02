/************************************************************************/
/*
	This is a final year project of sound synthesis on parallella board.
	Website: https://github.com/dzl199401/Modal-Synthesis-Parallella

	This is the head file of e_string.c
*/
/************************************************************************/
#ifndef EMSTRING_H
#define EMSTRING_H
#include "e_lib.h"
//#include "mymath.c"
#include "ms-data-device.h"

int e_string_output(mstring string,Spara* p_para);
int e_string_modes(mstring* n_string, Spara* p_para);
int e_string_modesUpdate(mstring* n_string, Spara* p_para);
#endif
