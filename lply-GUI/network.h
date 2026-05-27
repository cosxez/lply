#pragma once

void sclose(int *sock)
{
	if (*sock<0){return;}
	else{close(*sock);*sock=-1;}
}
short lply_sbatt(int *sock,struct sockaddr_in *addr,struct sockaddr_in *faddr,std::string ip,unsigned short port)
{
	try
	{
		addr->sin_family=AF_INET;
		addr->sin_port=htons(port);
		addr->sin_addr.s_addr=INADDR_ANY;
	
		bind(*sock,(struct sockaddr*)addr,sizeof(*addr));
	
		faddr->sin_family=AF_INET;
		faddr->sin_port=htons(port);
	
		inet_pton(AF_INET,ip.c_str(),&(faddr->sin_addr));
	}
	catch(std::exception &e){return -1;}
	return 0;
}

int lply_read(int *sock,struct sockaddr_in *faddr,void* pbuff,size_t sbuff)
{
	struct sockaddr_in ffaddr;
	socklen_t ips=sizeof(ffaddr);
	int sb=-1;
	for (unsigned short i=0;i<32;i++)
	{
		sb=recvfrom(*sock,pbuff,sbuff,0,(struct sockaddr*)&ffaddr,&ips);
		if (sb>0 && ffaddr.sin_addr.s_addr==faddr->sin_addr.s_addr){break;}
	}
	unsigned short mgc_chk=*(unsigned short*)pbuff;
	if (sb==2 && mgc_chk==0xe1dd){sb=-4;}
	return sb;
}

void lply_lconn(int *sock,struct sockaddr_in *faddr,char *is_conn)
{
	struct sockaddr_in ffaddr;
	socklen_t ips=sizeof(ffaddr);
	for (unsigned int i=0;i<32;i++)
	{
		unsigned short mgc;
		recvfrom(*sock,&mgc,sizeof(mgc),0,(struct sockaddr*)&ffaddr,&ips);
		if (ffaddr.sin_addr.s_addr==faddr->sin_addr.s_addr &&mgc==0xAC5F){*is_conn=1;break;return;}
	}
}

short lply_connect(int *sock,struct sockaddr_in *faddr)
{
	char is_conn=0;
	std::thread(lply_lconn,sock,faddr,&is_conn).detach();
	unsigned short mgc=0xC00F;
	for (unsigned short i=0;i<512;i++)
	{
		if (is_conn==1){break;}
		sendto(*sock,&mgc,sizeof(mgc),0,(struct sockaddr*)faddr,sizeof(*faddr));
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	if (is_conn==1){return 0;}
	return -1;
}

short crdt(int *sock,struct sockaddr_in *faddr,unsigned short mgr,void *wrd,size_t swrd)
{
	unsigned char tbuff[1024];
	unsigned short mngd=(0x07<<8 | (mgr & 0xff));
	for (unsigned short i=0;i<32;i++)
	{
		sendto(*sock,&mngd,sizeof(mngd),0,(struct sockaddr*)faddr,sizeof(*faddr));
		int csb=lply_read(sock,faddr,&tbuff,sizeof(tbuff));if (csb<2+swrd){return -1;}

		if ((*(unsigned short*)tbuff)==mgr)
		{
			memcpy(wrd,tbuff+2,swrd);
			break;
		}
	}
	return 0;
}

void lply_rmdap(int *sock,struct sockaddr_in *faddr,void *pbuff,size_t sbuff,ma_engine *eng,ma_decoder *decoder,ma_sound *sound)
{
	if (ma_sound_is_playing(sound)){ma_sound_stop(sound);}
	ma_sound_uninit(sound);
	ma_decoder_uninit(decoder);

	size_t crp=0;
	unsigned char nfdwr=0;
	while (crp<sbuff)
	{
		size_t csb=lply_read(sock,faddr,(char*)(pbuff)+crp,sbuff);
		if (csb==2 && *(unsigned short*)((char*)(pbuff)+crp)==0xe3dd){nfdwr=1;break;}
		if (csb<1){return;}
		crp+=csb;
	}
	if (nfdwr==0){unsigned short yaya;if (lply_read(sock,faddr,&yaya,sizeof(yaya))<1){return;}}

	ma_decoder_init_memory(pbuff,sbuff,NULL,decoder);
	ma_sound_init_from_data_source(eng,decoder,0,NULL,sound);
	ma_sound_start(sound);
}
