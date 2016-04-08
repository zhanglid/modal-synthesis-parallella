#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include "inc/threadMeasurement.h"
#include "inc/mstring.h"
#include "inc/ms-data-host.h"


void* threadMeasurement(void* p){
	mstring* (*p_ms)[4]=(mstring* (*)[4])p;
	volatile int* pnum=&sendnum;
	while(1){
		refresh();
		usleep(30*1000);
		clear();
		volatile memspace* pmem=(volatile memspace*)(p_ms[ms_index/4][ms_index%4]->pmem);
		volatile char* pmsg=(volatile char*)(p_ms[ms_index/4][ms_index%4]->pmsg);
		mvprintw(0,0,"sendtime:");
		attron(A_BOLD);
		mvprintw(0,10,"%.2fs", (float)((*pnum)*TCPSPEED/44100.0));
		attroff(A_BOLD);
		mvprintw(18,0,"Msg:%s",pmsg);

		mvprintw(9,0,"Force status:%c",pmem->syn[2]);
		mvprintw(11,0,"Fs:%d\tL:%f\ta:%e\tTe:%f\tE:%f\t",pmem->parameters.Fs,pmem->parameters.L,pmem->parameters.a,pmem->parameters.Te,pmem->parameters.E);
		mvprintw(12,0,"rho:%f\tb1:%f\tb2:%e\tre:%e\trp:%f\t",pmem->parameters.rho,pmem->parameters.b1,pmem->parameters.b2,pmem->parameters.re,pmem->parameters.rp);
		mvprintw(13,0,"T:%e\txe:%e\txp:%f\tA:%e\tr:%f",pmem->parameters.T,pmem->parameters.xe,pmem->parameters.xp,pmem->parameters.A,pmem->parameters.r);

		mvprintw(14,0,"I:%e\tc:%f\tkappa:%f",pmem->parameters.I,pmem->parameters.c,pmem->parameters.kappa);
		mvprintw(15,0,"version:%c\tstringIndex:%02d",pmem->parameters.version,ms_index+1);
		mvprintw(0,40,"%c %c %c %c",(ms_index/4==0&&ms_index%4==0)?'*':'.',(ms_index/4==0&&ms_index%4==1)?'*':'.',(ms_index/4==0&&ms_index%4==2)?'*':'.',(ms_index/4==0&&ms_index%4==3)?'*':'.');
		mvprintw(1,40,"%c %c %c %c",(ms_index/4==1&&ms_index%4==0)?'*':'.',(ms_index/4==1&&ms_index%4==1)?'*':'.',(ms_index/4==1&&ms_index%4==2)?'*':'.',(ms_index/4==1&&ms_index%4==3)?'*':'.');
		mvprintw(2,40,"%c %c %c %c",(ms_index/4==2&&ms_index%4==0)?'*':'.',(ms_index/4==2&&ms_index%4==1)?'*':'.',(ms_index/4==2&&ms_index%4==2)?'*':'.',(ms_index/4==2&&ms_index%4==3)?'*':'.');
		mvprintw(3,40,"%c %c %c %c",(ms_index/4==3&&ms_index%4==0)?'*':'.',(ms_index/4==3&&ms_index%4==1)?'*':'.',(ms_index/4==3&&ms_index%4==2)?'*':'.',(ms_index/4==3&&ms_index%4==3)?'*':'.');
		refresh();
	}
}
