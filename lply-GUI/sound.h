#pragma once

void lply_posct(ma_sound *sound)
{
	if (ma_sound_is_playing(sound)){ma_sound_stop(sound);return;}
	if (!ma_sound_is_playing(sound)){ma_sound_start(sound);}
}

char lply_capt(char *mlist,unsigned int mlistrm,unsigned int tgmi,unsigned int tlmi,char *flist,unsigned int flists,unsigned int tgfi,unsigned int mlisti,unsigned int mlistio,char *is_busy,char *ld,unsigned char **mbuff,unsigned int *sbuff,ma_engine *eng,ma_decoder *decoder,ma_sound *sound,unsigned long long int *mcp,int *sock,struct sockaddr_in *faddr)
{
	*is_busy=1;

	unsigned int smli=0;
	unsigned int spfml=0;
//	char *sfmi=NULL;
	for (unsigned int i=0;i<mlistrm;i++)
	{
		if (mlist[i]=='\n'){smli+=1;if (smli==(mlisti+mlistio)){/*sfmi=(char*)malloc(i-spfml);memcpy(sfmi,&mlist[spfml],i-spfml);*/break;}spfml=i+1;}
	}
	
	unsigned int sfli=0;
	unsigned int spffl=0;
	char *sfb=NULL;unsigned int sfbs=0;
	for (unsigned int i=0;i<flists;i++)
	{
		if (flist[i]=='\n'){sfli+=1;if (smli==sfli){sfb=(char*)malloc(i-spffl);memcpy(sfb,&flist[spffl],i-spffl);sfbs=i-spffl;break;}spffl=i+1;}
	}
	
	if (ma_sound_is_playing(sound)){ma_sound_stop(sound);}
	ma_sound_uninit(sound);
	ma_decoder_uninit(decoder);

	if (sfli<=tlmi){if (lply_gmdfld(mbuff,sbuff,ld,sfb)!=0){free(sfb);*is_busy=0;return -2;}}
	else {if (lply_gmdsfs(sock,faddr,sfb,sfbs,mbuff,sbuff)!=0){free(sfb);*is_busy=0;return -3;}lply_rmdaps(sock,faddr,*mbuff,*sbuff,sfb,sfbs);}
	
	free(sfb);
	*is_busy=0;

	ma_decoder_init_memory(*mbuff,*sbuff,NULL,decoder);
	ma_sound_init_from_data_source(eng,decoder,0,NULL,sound);
	ma_data_source_get_length_in_pcm_frames(ma_sound_get_data_source(sound),mcp);
	ma_sound_start(sound);
//	free(sfmi);
}
