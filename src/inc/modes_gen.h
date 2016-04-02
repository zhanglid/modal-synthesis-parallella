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
#ifndef MODESGEN_H
#define MODESGEN_H
#include "ms-spara.h"
#include "ms-data-device.h"
extern volatile memspace *pmem;
int e_string_modes(mstring* n_string, Spara* p_para);
int e_string_modesUpdate(mstring* n_string, Spara* p_para);
#endif
