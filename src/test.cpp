#include <netinet/in.h>  // for sockaddr_in
#include <sys/types.h>  // for socket
#include <sys/socket.h>  // for socket
#include <stdio.h>    // for printf
#include <stdlib.h>    // for exit
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main()
{
	int sockfd;
	struct sockaddr_in server_addr;
	char ip[]="192.168.137.1";
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htons(ip);
	
	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
			printf("inet_pton error for %s\n", ip);
			return -1;
		}
	
	
	
	server_addr.sin_port=htons(8859);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
		
		
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
			return -1;
		}
		
 
    //char  sendData[] = "Hello I'am TCP client!\n";
	float x[441]={0};
	
	for (int i=0;i<441;i++){
			x[i]=(float)i/100;
	}
	
	
	for (int i=0;i<50;i++){
		sleep(1);
		send(sockfd, x, 441*sizeof(float), 0);
	}
    char recData[255];
    int ret = recv(sockfd, recData, 255, 0);
    if(ret > 0)
    {
        recData[ret] = 0x00;
        printf("%s",recData);
    }
    close(sockfd);
    return 0;
}