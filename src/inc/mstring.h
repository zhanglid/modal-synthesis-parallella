#ifndef MSTRINGCLASS_H
#define MSTRINGCLASS_H
#include <stdio.h>
#include <e-hal.h>
#include <e-loader.h>
#include "ms-data-host.h"
class mstring
{
	public:
		mstring(unsigned _row, unsigned _col, Spara _sp);
		~mstring();
		int start();
		volatile memspace* pmem;
		volatile char* pmsg;
		Spara sp;
	private:
		unsigned row;
		unsigned col;
		e_epiphany_t dev;
		e_mem_t msg;
		e_mem_t mem;
};
#endif
