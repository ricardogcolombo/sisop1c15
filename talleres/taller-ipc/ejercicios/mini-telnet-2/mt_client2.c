#include "mt.h"

int main(int argc, char* argv[]) {
	/* Implementar */


	int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[MAX_MSG_LENGTH];
   char recvline[1000];

   if (argc != 2)
   {
      printf("usage:  client <IP address>\n");
      exit(1);
   }

   sockfd=socket(AF_INET,SOCK_STREAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = inet_addr(argv[1]);	
   servaddr.sin_port=htons(PORT);

   if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        perror("Conectando");
        exit(1);
   }

   while (fgets(sendline, MAX_MSG_LENGTH ,stdin) != NULL)
   {
      sendto(sockfd,sendline,strlen(sendline),0,
      (struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stdout);

      fflush(stdout);
      memset(&recvline,0,MAX_MSG_LENGTH);
      //fflush(sendline);

   }

	return 0;
}

