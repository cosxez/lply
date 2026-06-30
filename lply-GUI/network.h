#pragma once

void sclose(int *sock)
{
	if (*sock<0){return;}
	close(*sock);*sock=-1;
}

int lply_read(int *sock,struct sockaddr_in *faddr,void* pbuff,size_t sbuff,int flags)
{
	struct sockaddr_in ffaddr;
	socklen_t ips=sizeof(struct sockaddr_in);
	int sb=-1;
	sb=recvfrom(*sock,pbuff,sbuff,flags,(struct sockaddr*)&ffaddr,&ips);
	if (sb>0 && ffaddr.sin_addr.s_addr==faddr->sin_addr.s_addr){return sb;}

	unsigned short mgc_chk=*(unsigned short*)pbuff;
	if (sb==2 && mgc_chk==0xe1dd){sb=-4;}
	return sb;
}

char lply_sbatt(int *sock,struct sockaddr_in* addr,struct sockaddr_in* faddr,char* ip,unsigned short port)
{
	*sock=socket(AF_INET,SOCK_DGRAM,0);
	addr->sin_family=AF_INET;
	addr->sin_port=htons(port);
	addr->sin_addr.s_addr=INADDR_ANY;
	bind(*sock,(struct sockaddr*)addr,sizeof(*addr));

	faddr->sin_family=AF_INET;
	faddr->sin_port=htons(port);
	if (inet_pton(AF_INET,ip,&(faddr->sin_addr))!=1){return -2;}
	return 0;
}

short lply_gmlfls(int *sock,struct sockaddr_in *faddr,char **mlist,unsigned int *mlists,unsigned int *tmi,char **flist,unsigned int *flists,unsigned int *tfi)
{
	unsigned short mgcfgml=0xa1b3;
	struct timespec dl;dl.tv_sec=1;dl.tv_nsec=0;
	for (unsigned char i=0;i<16;i++)
	{
		sendto(*sock,&mgcfgml,sizeof(mgcfgml),0,(struct sockaddr*)faddr,sizeof(struct sockaddr_in));

		thrd_sleep(&dl,NULL);
		size_t mlistrm=0;
		if (i==15){return -322;}
		if (lply_read(sock,faddr,&mlistrm,sizeof(mlistrm),MSG_DONTWAIT)<1){continue;}
		if (mlistrm==0){continue;}
		
		*mlist=(char*)malloc(mlistrm);
		size_t crp=0;
		unsigned char nfdwr=0;
		while (crp<mlistrm)
		{
			char tbuff[1024];
			size_t csb=lply_read(sock,faddr,tbuff,1024,0);
			if (csb<1){sclose(sock);return -2;}
			if (csb==2 && *(unsigned short*)tbuff==0xe3dd){nfdwr=1;break;}
			memcpy(*mlist+crp,tbuff,csb);
			crp+=csb;
		}
		if (nfdwr==0){unsigned short yaya;if (lply_read(sock,faddr,&yaya,2,0)<1){sclose(sock);return -2;}}
		*mlists=mlistrm;
		for (unsigned int i=0;i<*mlists;i++){if ((*mlist)[i]=='\n'){*tmi+=1;}}

		size_t flistrm=0;
		if (lply_read(sock,faddr,&flistrm,sizeof(flistrm),0)<1){sclose(sock);return -2;}
		if (flistrm==0){sclose(sock);return -2;}
		*flist=(char*)malloc(flistrm);
		crp=0;
		nfdwr=0;
		while (crp<flistrm)
		{
			char tbuff[1024];
			size_t csb=lply_read(sock,faddr,tbuff,1024,0);
			if (csb<1){sclose(sock);return -2;}
			if (csb==2 && *(unsigned short*)tbuff==0xe3dd){nfdwr=1;break;}
			memcpy(*flist+crp,tbuff,csb);
			crp+=csb;
		}
		if (nfdwr==0){unsigned short yaya;if (lply_read(sock,faddr,&yaya,2,0)<1){sclose(sock);return -2;}}
		*flists=flistrm;
		for (unsigned int i=0;i<*flists;i++){if ((*flist)[i]=='\n'){*tfi+=1;}}
		break;
	}
	return 0;
}

short lply_gmdsfs(int *sock,struct sockaddr_in *faddr,char* sfb,unsigned int sfbs,unsigned char **mbuff,unsigned int *sbuff,unsigned int *mds)
{
	unsigned char bfgmd[2+sfbs];
	*(unsigned short*)bfgmd=0x65f1;
	memcpy(&bfgmd[2],sfb,sfbs);

	size_t bsfm=0;
	struct timespec dl;dl.tv_sec=1;dl.tv_nsec=0;
	for (unsigned char i=0;i<16;i++)
	{
		sendto(*sock,&bfgmd,sizeof(bfgmd),0,(struct sockaddr*)faddr,sizeof(struct sockaddr_in));
		thrd_sleep(&dl,NULL);
		char tbuff[10];
		if (lply_read(sock,faddr,&tbuff,sizeof(tbuff),MSG_DONTWAIT)==10 && *(unsigned short*)tbuff==0xa5f1){memcpy(&bsfm,&tbuff[2],8);break;}
		if (i==15){return -322;}
	}
	if (bsfm>*sbuff){unsigned char *tmp=realloc(*mbuff,bsfm);if (tmp==NULL){return -1;}*mbuff=tmp;*sbuff=bsfm;}

	*mds=bsfm;
	return 0;
}

void lply_rmdaps(int *sock,struct sockaddr_in *faddr,void *pbuff,unsigned int sbuff,char* sfb,unsigned int sfbs,unsigned int* nls)
{
	size_t crp=0;

	unsigned char bfgmd[8+sfbs];
	*(unsigned short*)bfgmd=0x6e9d;
	memcpy(&bfgmd[2],sfb,sfbs);
	while (crp<sbuff)
	{
		*(unsigned int*)(bfgmd+2+sfbs)=crp;
		if ((crp+65535)<=sbuff){*(unsigned short*)(bfgmd+6+sfbs)=65535;}
		else{*(unsigned short*)(bfgmd+6+sfbs)=sbuff-crp;}

		sendto(*sock,&bfgmd,sizeof(bfgmd),0,(struct sockaddr*)faddr,sizeof(struct sockaddr_in));
		size_t ccrp=0;
		unsigned char nfdwr=0;
		while (ccrp<*(unsigned short*)(bfgmd+6+sfbs))
		{
			unsigned char tb[*(unsigned short*)(bfgmd+6+sfbs)];
			size_t csb=lply_read(sock,faddr,&tb,sizeof(tb),0);
			if (csb<1){return;}
			if (csb==2 && *(unsigned short*)tb==0xe3dd){nfdwr=1;break;}
			if (*(unsigned int*)tb + *(unsigned short*)(tb+4)<=sbuff){memcpy(((char*)(pbuff) + *(unsigned int*)tb),&tb[6],*(unsigned short*)(tb+4));}
			ccrp+=*(unsigned short*)(tb+4);
		}
		if (nfdwr==1){continue;}
		if (nfdwr==0){unsigned short yaya;if (lply_read(sock,faddr,&yaya,2,0)<1){return;}}
		
		crp+=ccrp;
		*nls=crp;
	}
}
