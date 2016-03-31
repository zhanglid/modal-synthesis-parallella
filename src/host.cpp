#define DATATYPE float
#define PI 3.1416
#define INPSIZE 10
#define DEBUG 0
#define TCPSPEED 512
#include <iostream>
#include <string>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <e-hal.h>
#include <stdlib.h>
#include <stdio.h>
#include <e-loader.h>
#include <fstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <ncurses.h>
#include <math.h>
using namespace std;
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
	volatile int none;
	volatile float data[TCPSPEED];
	volatile char syn[3];
	volatile char paraUpdate;
	volatile char force;
	volatile unsigned num;
	volatile char msg[128];
} memspace;


struct timeval tv_begin, tv_end;
struct timeval tv2_begin, tv2_end;
int flag = 0;
int num = 0;
volatile unsigned ms_index = 0;
int sendnum = 0;
volatile bool corestatus[4][4] = {0};
class mstring
{
	public:
		mstring(unsigned _row, unsigned _col, Spara _sp) {
			sp = _sp;
			row = _row;
			col = _col;
			if (E_ERR==e_open(&dev, row, col, 1, 1)){
					cout<<"e_open failed"<<endl;
			}
			if	(E_ERR==e_reset_group(&dev)){
					cout<<"e_reset_group failed"<<endl;
			}
			char name[10];
			snprintf(name,10,"_%dmStr%d", row, col);
			int rc = e_shm_alloc(&mem, name, sizeof(memspace));//Data structure: [buffer][r/w parameters]
				if (rc == E_ERR) {
					if (E_ERR==e_shm_attach(&mem, name)){
							cout<<"e_shm_attach failed"<<endl;
					}
				}
				/*snprintf(name,10,"_%dmsg%d", row, col);
				rc = e_shm_alloc(&msg, name, 128);//Data structure: [buffer][r/w parameters]
				//	rc = e_alloc(&msg,0x01000000+0x100000*(row*4+col), 0x100000);
					if (rc == E_ERR) {
						if (E_ERR==e_shm_attach(&msg, name)){
								cout<<"e_shm_attach failed"<<endl;
						}
					}*/

					pmem=(volatile memspace*)mem.base;
					pmsg=pmem->msg;
					snprintf((char*)pmsg,20,"%d|%d",row,col);
					if (DEBUG){
						fprintf(stderr, "mstring::msg.base:%08x,mem.base:%08x,sizeof(memspace):%d\n",pmsg,pmem,sizeof(memspace));
					}
			//	cout<<"after shm"<<row<<"|"<<col<<endl;
				if (E_OK != e_load("e_mstring.srec", &dev, 0, 0, E_FALSE)) {
					cout << "Failed to load e_hello_world.srec" << endl;
				}
				//sleep(3);

				pmem->paraUpdate='n';
				pmem->syn[0]='n';
				pmem->force='n';

				do {
				memcpy((void*)&(pmem->parameters),(void*)&sp,sizeof(Spara));
			}while(pmem->parameters.version!=sp.version);
		}
		~mstring() {
			e_close(&dev);
		}
		//status msg;
		int start() {
			if (DEBUG) {
			//	cout << "start()" << row << "|" << col << endl;
				fprintf(stderr,"start(%d,%d)\n",row, col);
			}
			if (E_ERR==e_start_group(&dev)){
					cout<<"e_start error"<<endl;
					sleep(10);
			}
			return 1;
		}
		e_epiphany_t dev;
		volatile memspace* pmem;
		volatile char* pmsg;
		Spara sp;
		e_mem_t mem;
	private:
		unsigned row;
		unsigned col;

		e_mem_t msg;
};

class tcpclient
{
	public:
	tcpclient(const char* ip, int _port) {
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
			return;
		}
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(_port);
		if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
			printf("inet_pton error for %s\n", ip);
			return;
		}
	}
	int startConnect(){

		if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
			printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
		return 0;
	}
	int sendData(DATATYPE *p, int num) {
		int rc;
		if (rc = send(sockfd, p, num*sizeof(float), 0) < 0)
		{
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
		return rc;
	}
	int recvOK(){
		char x=0;
		int rc=0;
		while (rc = recv(sockfd, &x, 1, 0) < 0)
		{
			printf("recv msg error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}

		if(x == 'y'){
			return 1;
		}
		return 0;
	}
	private:
		//DATATYPE buf[SENDSIZE];
		int sockfd;
		struct sockaddr_in    servaddr;
};

typedef struct _threadTCPdata{
	tcpclient* p_tcp;
	DATATYPE* p_buffer;
}threadTCPdata;

void* threadForce(void *_ms){
	mstring* (*p_ms)[4]=(mstring* (*)[4])_ms;
	volatile memspace *pmem=(volatile memspace *)(p_ms[0][0]->pmem);
	volatile char* p_ms_force=&(pmem->force);
	char ch;
	while(1){
	//	mvprintw(16,0,"Now control string: No.%02d",ms_index+1);
		ch = getch();
		if (ch == 'f'){
			//gettimeofday(&tv_begin,NULL);
			while(*p_ms_force!='y') p_ms_force[0]='y';
			//mvprintw(1,0,"Force press: %d",++num);
			flag=1;
		}
		if(ch == 's'){
			flag=0;
		}
		if(ch == 'q'){
			ms_index = (ms_index+1)%16;
			pmem = (volatile memspace *) p_ms[ms_index/4][ms_index%4]->pmem;
			p_ms_force=&(pmem->force);
		}
		if(ch=='p'){
			float t=pmem->parameters.L;
			char v=pmem->parameters.version;
			v++;
			v='a'+(v-'a')%26;
			t*=0.99;
			while(pmem->parameters.L!=t) pmem->parameters.L=t;
			while(pmem->parameters.version!=v) pmem->parameters.version=v;
			while(pmem->paraUpdate!='y') pmem->paraUpdate='y';
			//mvprintw(10,0,"Para press,change the length to:%.3f",pmem->parameters.L);
		}

		if (ch=='n'){
			float t=pmem->parameters.b1;
			char v=pmem->parameters.version;
			v++;
			v='a'+(v-'a')%26;
			t*=0.99;
			while(pmem->parameters.b1!=t) pmem->parameters.b1=t;
			while(pmem->parameters.version!=v) pmem->parameters.version=v;
			while(pmem->paraUpdate!='y') pmem->paraUpdate='y';
			//mvprintw(10,0,"freq-independent damping(b1) to:%.3f",pmem->parameters.b1);
		}
		if (ch=='m'){
			float t=pmem->parameters.b2;
			char v=pmem->parameters.version;
			v++;
			v='a'+(v-'a')%26;
			t*=0.99;
			while(pmem->parameters.b2!=t) pmem->parameters.b2=t;
			while(pmem->parameters.version!=v) pmem->parameters.version=v;
			while(pmem->paraUpdate!='y') pmem->paraUpdate='y';
			//mvprintw(10,0,"freq-independent damping(b2) to:%.3f",pmem->parameters.b2);
		}
		usleep(1000);
	}
}

void* threadMeasurement(void* p){
	mstring* (*p_ms)[4]=(mstring* (*)[4])p;
	volatile int* pnum=&sendnum;
	while(1){
		usleep(30*1000);
		//clear();
		//mvprintw(0,10,"00000+%d,(%d,%d)",ms_index,ms_index/4,ms_index%4);
		//refresh();
		volatile memspace* pmem=(volatile memspace*)(p_ms[ms_index/4][ms_index%4]->pmem);
		//mvprintw(0,10,"11111+%d,(%d,%d)",ms_index,ms_index/4,ms_index%4);
		//refresh();
		volatile char* pmsg=(volatile char*)(p_ms[ms_index/4][ms_index%4]->pmsg);
	//	mvprintw(0,10,"22222+%d,(%d,%d)",ms_index,ms_index/4,ms_index%4);
		refresh();

		//clear();
		mvprintw(0,0,"sendtime:");
		attron(A_BOLD);
		mvprintw(0,10,"%.2fs", (float)((*pnum)*TCPSPEED/44100.0));
		attroff(A_BOLD);
		mvprintw(18,0,"Msg:%s",pmsg);

		mvprintw(9,0,"Force status:%c",pmem->syn[2]);
		//mvprintw(0,10 ,"33333+%d,(%d,%d)",ms_index,ms_index/4,ms_index%4);
	//	refresh();
	/*memspace* test=NULL;
		mvprintw(17,0,"[0x%08x]",(test->msg));
	refresh();*/
		//if (ms_index%2==1) sleep(20);
		mvprintw(11,0,"Fs:%d\tL:%f\ta:%e\tTe:%f\tE:%f\t",pmem->parameters.Fs,pmem->parameters.L,pmem->parameters.a,pmem->parameters.Te,pmem->parameters.E);
	//	mvprintw(0,10,"44444+%d,0x%08x",ms_index,pmem);
		//refresh();
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

FILE  *diag_fd=fopen( "diag_fd.out", "w");
int main() {
	FILE *stream = freopen( "log.out", "w", stderr );
	//FILE *stream1 = freopen( "diag_fd.out", "w", diag_fd );

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
	//sp1.T = 1/(float)sp1.Fs;
	// sp1.xe = sp1.re*sp1.L;
	// sp1.xp = sp1.rp*sp1.L;
	// sp1.A = PI*sp1.a*sp1.a;
	//
	// sp1.r = sp1.a / 2;
	// sp1.I = sp1.A*sp1.r*sp1.r;
	// sp1.c = sqrt(sp1.Te / (sp1.rho*sp1.A));
	// sp1.kappa = sqrt((sp1.E*sp1.I) / (sp1.rho*sp1.A));
	// sp1.multiV = 2.0f / sp1.L * Fs;
	// sp1.multiF = sp1.multiV;
	// sp1.version = 'a';
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	mstring* p_ms[4][4];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			p_ms[i][j] = new mstring( i, j, sp[(4*i+j)%6] );

			//mvprintw(0,0,"%d,%d,%08x", i, j, p_ms[i][j]->mem.base);
		//	refresh();
			//getchar();
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
	//cout<<"nonono"<<endl;
	//sleep(5);
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
								//corestatus[i][j]=0;
								AllSyn=0;
						}
						//corestatus[i][j]=1;
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
	fclose( stream );
	//fclose( stream1 );
	return 0;
}
