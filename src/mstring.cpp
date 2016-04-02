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

      pmem=(volatile memspace*)mem.base;
      pmsg=pmem->msg;
      snprintf((char*)pmsg,20,"%d|%d",row,col);
      if (DEBUG){
        fprintf(stderr, "mstring::msg.base:%08x,mem.base:%08x,sizeof(memspace):%d\n",(unsigned)pmsg, (unsigned)pmem,sizeof(memspace));
      }
    if (E_OK != e_load("e_mstring.srec", &dev, 0, 0, E_FALSE)) {
      cout << "Failed to load e_hello_world.srec" << endl;
    }
    pmem->paraUpdate='n';
    pmem->syn[0]='n';
    pmem->force='n';
    do {
    memcpy((void*)&(pmem->parameters),(void*)&sp,sizeof(Spara));
  }while(pmem->parameters.version!=sp.version);
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
