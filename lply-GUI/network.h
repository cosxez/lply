#pragma once

void sclose(int *sock)
{
	if (*sock<0){return;}
	close(*sock);*sock=-1;
}

char lply_sbatt(int *sock,struct sockaddr_in* addr,struct sockaddr_in* faddr,char* ip,unsigned short port)
{
	*sock=socket(AF_INET,SOCK_DGRAM,0);
	addr->sin_family=AF_INET;
	addr->sin_port=htons(port);
	addr->sin_addr.s_addr=INADDR_ANY;
	if (bind(*sock,(struct sockaddr*)addr,sizeof(*addr))!=0){return -1;}

	faddr->sin_family=AF_INET;
	faddr->sin_port=htons(port);
	if (inet_pton(AF_INET,ip,&(faddr->sin_addr))!=1){return -2;}
	return 0;
}
