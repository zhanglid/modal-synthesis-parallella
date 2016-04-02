#include <iostream>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "inc/tcpclient.h"

tcpclient::tcpclient(const char* ip, int _port) {
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
      return;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(_port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
      printf("inet_pton error for %s\n", ip);
    }
}

int tcpclient::startConnect(){
  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
    return -1;
  }
  return 0;
}

int tcpclient::sendData(DATATYPE *p, int num) {
  int rc;
  if (rc = send(sockfd, p, num*sizeof(float), 0) < 0)
  {
    printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
    return -1;
  }
  return rc;
}

int tcpclient::recvOK(){
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
