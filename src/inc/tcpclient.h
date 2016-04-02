#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <iostream>
#include <stdio.h>
#include "ms-data-host.h"
class tcpclient
{
	public:
    tcpclient(const char* ip, int _port);
    int startConnect();
    int sendData(DATATYPE *p, int num);
    int recvOK();
  private:
    int sockfd;
    struct sockaddr_in    servaddr;
};
#endif
