#include <stdio.h>
#include <unistd.h>
#include <e-hal.h>
#include <math.h>
#include <pthread.h>
#include <ncurses.h>
#include "inc/host.h"

int main() {
/*	if (DEBUG) {
		FILE *stream = freopen( "log.out", "w", stderr );
	}*/
	//init the platform//////////////////////////////////////////////////////////////////////////////////////////
	e_platform_t platform;
	int rc;
	unsigned coreId = (0 + platform.row) * 64 + 0 + platform.col;
	e_set_loader_verbosity(L_D0);
	e_set_host_verbosity(H_D0);
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
	///init_interface////////////////////////////////////////////////////////////////////////////////////////////
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Spara* sp = new Spara[6];
	sp[0] = {44100,1.0,2e-4,9,7.8e9,2.6e3,0.5,5e-4,0.05,0.83,0,0,0,0,0,0,0,0};
	sp[1] = {44100,1.0,2e-4,16,7.8e9,2.6e3,0.5,5e-4,0.05,0.83,0,0,0,0,0,0,0,0};
	sp[2] = {44100,1.0,2e-4,29,7.8e9,2.6e3,0.5,5e-4,0.05,0.83,0,0,0,0,0,0,0,0};
	sp[3] = {44100,1.0,2e-4,50,7.8e9,2.6e3,0.5,5e-4,0.05,0.83,0,0,0,0,0,0,0,0};
	sp[4] = {44100,1.0,2e-4,79,7.8e9,2.6e3,0.5,5e-4,0.05,0.83,0,0,0,0,0,0,0,0};
	sp[5] = {44100,1.0,2e-4,142,7.8e9,2.6e3,0.5,5e-4,0.05,0.83,0,0,0,0,0,0,0,0};
	genPara(&sp[0]);
	genPara(&sp[1]);
	genPara(&sp[2]);
	genPara(&sp[3]);
	genPara(&sp[4]);
	genPara(&sp[5]);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	mstring* p_ms[4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			p_ms[i][j] = new mstring( i, j, sp[(4*i+j)%6] );
		}
	}
	delete sp;
	//init the tcp transmission///////////////////////////////////////////////////////////////////////////////////
	const char ip[] = "192.168.137.1";
	tcpclient tcpsend(ip, 8859);
	mvprintw(0,0,"Waiting for connection to %s",ip);
	refresh();
	while (tcpsend.startConnect() == -1) {
		printf("failed to connect to host[%s]!\n",ip);
		usleep(100*1000);
	}
	mvprintw(0,0,"                                                                                          ");
	refresh();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int i=0;i<4;i++){
		for (int j = 0; j < 4; j++) {
			p_ms[i][j]->start();
		}
	}
	long long count = 0;
	pthread_t t_measure,t_force;
	pthread_create(&t_measure,NULL,threadMeasurement,(void*)p_ms);
	pthread_create(&t_force,NULL,threadForce,(void*)p_ms);

	while (1)
	{
			float outputNow = 0;
			unsigned AllSyn=0;
			while(AllSyn==0){
				AllSyn=1;
				for (size_t i = 0; i < 4; i++) {
					for (size_t j = 0; j < 4; j++) {
						if (p_ms[i][j]->pmem->syn[0]!='y'){
								AllSyn=0;
						}
					}
				}
					usleep(10);
			}
			float tmp[TCPSPEED]={0};
			for (size_t i = 0; i < 4; i++) {
				for (size_t j = 0; j < 4; j++) {
					for (size_t n = 0; n < TCPSPEED; n++) {
						tmp[n]+=p_ms[i][j]->pmem->data[n];
					}
				}
			}
			tcpsend.sendData(tmp, TCPSPEED);
			for (size_t i = 0; i < 4; i++){
				for (size_t j = 0; j < 4; j++) {
					while(p_ms[i][j]->pmem->syn[0]!='n') p_ms[i][j]->pmem->syn[0]='n';
			}
		}

			sendnum++;
			while(tcpsend.recvOK()!=1);
				usleep(100);
	}
	pthread_join(t_measure,NULL);
	pthread_join(t_force,NULL);
/*	if (DEBUG) {
		fclose(stream);
	}*/
	return 0;
}

void genPara(Spara* p_para){
	p_para->T = 1/(float)p_para->Fs;
	p_para->xe = p_para->re*p_para->L;
	p_para->xp = p_para->rp*p_para->L;
	p_para->A = PI*p_para->a*p_para->a;
	p_para->r = p_para->a / 2;
	p_para->I = p_para->A*p_para->r*p_para->r;
	p_para->c = sqrt(p_para->Te / (p_para->rho*p_para->A));
	p_para->kappa = sqrt((p_para->E*p_para->I) / (p_para->rho*p_para->A));
	p_para->multiV = 2.0f / p_para->L * p_para->Fs / 150;
	p_para->multiF = p_para->multiV /  17000;
	p_para->version = 'a';
}
