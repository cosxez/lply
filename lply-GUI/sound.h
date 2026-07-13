#pragma once

void lply_posct(ma_sound *sound)
{
	if (ma_sound_is_playing(sound)){ma_sound_stop(sound);return;}
	if (!ma_sound_is_playing(sound)){ma_sound_start(sound);}
}

void lply_mttit(ma_engine *eng,ma_sound *sound,short range,unsigned long long int mcp)
{
	unsigned long long int ccp;ma_sound_get_cursor_in_pcm_frames(sound,&ccp);

	long long int offset=(long long int)ma_engine_get_sample_rate(eng)*range;
	if (offset>0 && offset+ccp>=mcp){ma_sound_seek_to_pcm_frame(sound,0);ma_sound_seek_to_pcm_frame(sound,mcp);return;}
	if (offset<0 && -offset>ccp){ma_sound_seek_to_pcm_frame(sound,0);return;}
	ma_sound_seek_to_pcm_frame(sound,ccp+offset);
}

char lply_capt(SRAMm *rmb,unsigned short rmbs,char *mlist,unsigned int mlistrm,unsigned int tgmi,unsigned int tlmi,char *flist,unsigned int flists,unsigned int tgfi,unsigned int mlisti,unsigned int mlistio,char *is_busy,char *ld,unsigned char **mbuff,unsigned int *sbuff,unsigned int *mds,ma_engine *eng,ma_decoder *decoder,ma_sound *sound,unsigned long long int *mcp,int *sock,struct sockaddr_in *faddr,unsigned int* nls,char mfd)
{
	*is_busy=1;

	unsigned int smli=0;
	unsigned int spfml=0;
	for (unsigned int i=0;i<mlistrm;i++)
	{
		if (mlist[i]=='\n'){smli+=1;if (smli==(mlisti+mlistio)){break;}spfml=i+1;}
	}
	unsigned int sfli=0;
	unsigned int spffl=0;
	char *sfb=NULL;unsigned int sfbs=0;
	for (unsigned int i=0;i<flists;i++)
	{
		if (flist[i]=='\n'){sfli+=1;if (smli==sfli){sfb=(char*)malloc(i-spffl+1);memcpy(sfb,&flist[spffl],i-spffl);sfb[i-spffl]='\0';sfbs=i-spffl;break;}spffl=i+1;}
	}
	
	*nls=0;
	if (sfli<=tlmi){if (ma_sound_is_playing(sound)){ma_sound_stop(sound);}ma_sound_uninit(sound);ma_decoder_uninit(decoder);if (lply_gmdfld(mbuff,sbuff,ld,sfb,mds)!=0){free(sfb);*is_busy=0;return -2;}}
	else {if (lply_gmdsfs(sock,faddr,sfb,sfbs,mbuff,sbuff,mds)!=0){free(sfb);*is_busy=0;return -3;}if (ma_sound_is_playing(sound)){ma_sound_stop(sound);}ma_sound_uninit(sound);ma_decoder_uninit(decoder);if (mfd==0){lply_trmd(sock,faddr,*mbuff,*mds,sfb,sfbs,nls);}if (mfd==1){lply_rmdaps(sock,faddr,*mbuff,*mds,sfb,sfbs,nls);}}
	
	free(sfb);

	ma_decoder_init_memory(*mbuff,*mds,NULL,decoder);
	ma_sound_init_from_data_source(eng,decoder,0,NULL,sound);
	ma_data_source_get_length_in_pcm_frames(ma_sound_get_data_source(sound),mcp);
	
	*is_busy=0;
	ma_sound_start(sound);
}

char lply_ptfr(SRAMm *rmb,unsigned short rmbs,unsigned int tidx,unsigned char **mbuff,unsigned int *sbuff,unsigned int *mds,unsigned long long int *mcp,ma_engine *eng,ma_sound *sound,ma_decoder *decoder,char *is_busy)
{
	*is_busy=1;
	if (ma_sound_is_playing(sound)){ma_sound_stop(sound);}
	ma_sound_uninit(sound);ma_decoder_uninit(decoder);

	*mds=rmb[tidx].mbs;if (*mds>*sbuff){unsigned char* tmp=(unsigned char*)realloc(*mbuff,*mds);if (tmp==NULL){return -4;}memcpy(*mbuff,rmb[tidx].mba,rmb[tidx].mbs);}else{memcpy(*mbuff,rmb[tidx].mba,rmb[tidx].mbs);}

	ma_decoder_init_memory(*mbuff,*mds,NULL,decoder);
	ma_sound_init_from_data_source(eng,decoder,0,NULL,sound);
	ma_data_source_get_length_in_pcm_frames(ma_sound_get_data_source(sound),mcp);
	
	*is_busy=0;
	ma_sound_start(sound);
}

void lply_nxtt(unsigned int *cidx,unsigned int mlists)
{
	if (*cidx+1<=mlists){*cidx+=1;return;}*cidx=1;
}
