#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "inc/ms-data-host.h"
#include "inc/mstring.h"
using namespace std; /* std */
mstring::mstring(unsigned _row, unsigned _col, Spara _sp) {
  sp = _sp;
  row = _row;
  col = _col;
  extern float *guitar_data;
  if (E_ERR==e_open(&dev, row, col, 1, 1)){
      cout<<"e_open failed"<<endl;
  }
  if	(E_ERR==e_reset_group(&dev)){
      cout<<"e_reset_group failed"<<endl;
  }
  char name[10];
  snprintf(name,10,"_%dmStr%d", row, col);
  int rc = e_shm_alloc(&mem, name, MEMSIZE);//Data structure: [buffer][r/w parameters]
    if (rc == E_ERR) {
      if (E_ERR==e_shm_attach(&mem, name)){
          cout<<"e_shm_attach failed"<<endl;
      }
    }
  snprintf(name,10,"_%dmInp%d", row, col);
  rc = e_shm_alloc(&inpmem, name, sizeof(float)*INPSIZE);
      if (rc == E_ERR) {
        if (E_ERR==e_shm_attach(&inpmem, name)){
            cout<<"e_shm_attach failed"<<endl;
        }
      }
      volatile void* mem_addr = (volatile void*)mem.base;
      pmsg = (volatile char*)((char*)mem_addr + MSG_OFF);
      pmem.parameters = (volatile Spara*)((char*)mem_addr + PARA_OFF);
      pmem.data = (float*)((char*)mem_addr + DATA_OFF);
      pmem.syn = (volatile char*)((char*)mem_addr + SYN_OFF);
      pmem.paraUpdate = (volatile char*)((char*)mem_addr + PARAUP_OFF);
      pmem.force = (volatile char*)((char*)mem_addr + FORCE_OFF);
      pmem.reset = (volatile char*)((char*)mem_addr + RESET_OFF);
      pmem.num = (volatile unsigned *)((char*)mem_addr + NUM_OFF);
      pmem.msg = (volatile char*)((char*)mem_addr + MSG_OFF);
      pinp = (volatile float*)inpmem.base;
      do {
        memcpy((void *)pinp, guitar_data, INPGUITARLENGTH * sizeof(float));
      } while(pinp[100] != guitar_data[100]);
      if (DEBUG){
        fprintf(stderr, "mstring::msg.base:%08x,mem.base:%08x,sizeof(memspace):%d\n",(unsigned)pmsg, (unsigned)&pmem,MEMSIZE);
      }
    if (E_OK != e_load("e_mstring.srec", &dev, 0, 0, E_FALSE)) {
      cout << "Failed to load e_hello_world.srec" << endl;
    }

    pmem.paraUpdate[0]='n';
    pmem.syn[0]='n';
    pmem.force[0]='n';
    pmem.reset[0]='n';
    do {
      memcpy((void*)(pmem.parameters),(void*)&sp,DATA_OFF*sizeof(float));
  }while(pmem.parameters->version != sp.version);
}

  mstring::~mstring() {
  	e_close(&dev);
}

int mstring::start(){
  if (DEBUG) {
    fprintf(stderr,"start(%d,%d)\n",row, col);
  }
  if (E_ERR==e_start_group(&dev)){
      cout<<"e_start error"<<endl;
      sleep(10);
  }
  return 1;
}
