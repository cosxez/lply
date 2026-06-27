#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "structf.h"
#include "cfg_pars.h"
#include "fwwt.h"
#include "display.h"
#include "network.h"
#include "lmdr.h"
#include "sound.h"

int main()
{
	struct cnf conf;conf.thie=0;conf.wm=0;memset(&(conf.tc),0,3);
	if (cfg_pars(&conf)!=0){return -1;}
	
	int sock=-1;struct sockaddr_in addr;struct sockaddr_in faddr;memset(&addr,0,sizeof(addr));memset(&faddr,0,sizeof(faddr));
	
	struct dbs debug_struct;
	debug_struct.nstate=0;
	debug_struct.lstate=0;

	if (conf.wm==0)
	{
		if (lply_sbatt(&sock,&addr,&faddr,conf.ip,conf.port)!=0){sclose(&sock);debug_struct.nstate=0;}
		else{debug_struct.nstate=1;}
		if (lply_sbl(conf.lmd)!=0){debug_struct.lstate=0;}
		else{debug_struct.lstate=1;}
	}
	if (conf.wm==1){if (lply_sbatt(&sock,&addr,&faddr,conf.ip,conf.port)!=0){sclose(&sock);debug_struct.nstate=0;}else{debug_struct.nstate=1;}}
	if (conf.wm==2){if (lply_sbl(conf.lmd)!=0){debug_struct.lstate=0;}else{debug_struct.lstate=1;}}
	
	int win_width=864;
	int win_height=576;

	SDL_Window *win=SDL_CreateWindow("lply",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,win_width,win_height,SDL_WINDOW_SHOWN);
	SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);

	char run=1;
	SDL_Event ev;
	
	unsigned char *bfbg=NULL;
	unsigned char *bff=NULL;
	unsigned int bffs;
	if (lply_sthm(conf.bgi,conf.thie,&bfbg,&bff,&bffs)!=0){return -2;}
	SDL_Texture *btex=NULL;btex=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,*(unsigned int*)bfbg,*(unsigned int*)(bfbg+4));unsigned int tex_width=*(unsigned int*)bfbg;unsigned int tex_height=*(unsigned int*)(bfbg+4);
	lply_sa(ren,btex,bfbg);
	
	unsigned char sfb[]={0xf1,0x31,0x05,0x7b,0x8b,0x58,0x87,0x87,0xa8,0x85,0x46,0x85,0xb6};
	unsigned char font_size=1;

	char *nmlist=NULL;unsigned int nmlists=0;unsigned int tnmi=0;
	char *nflist=NULL;unsigned int nflists=0;unsigned int tnfi=0;
	if (debug_struct.nstate==1){if (lply_gmlfls(&sock,&faddr,&nmlist,&nmlists,&tnmi,&nflist,&nflists,&tnfi)!=0){debug_struct.nstate=0;}}

	char *lmlist=NULL;unsigned int lmlists=0;unsigned int tlmi=0;
	char *lflist=NULL;unsigned int lflists=0;unsigned int tlfi=0;
	if (debug_struct.lstate==1){lply_gmlfld(&lmlist,&lmlists,&tlmi,&lflist,&lflists,&tlfi,conf.lmd);}
	
	char *gmlist=NULL;unsigned int gmlists=0;unsigned int tgmi=0;gmlists=lmlists+nmlists;tgmi=tlmi+tnmi;gmlist=(char*)malloc(gmlists);if (lmlist!=NULL && lmlists>0){memcpy(gmlist,lmlist,lmlists);}if (nmlist!=NULL && nmlists>0){memcpy(&gmlist[lmlists],nmlist,nmlists);}
	char *gflist=NULL;unsigned int gflists=0;unsigned int tgfi=0;gflists=lflists+nflists;tgfi=tlfi+tnfi;gflist=(char*)malloc(gflists);if (lflist!=NULL && lflists>0){memcpy(gflist,lflist,lflists);}if (nflist!=NULL && nflists>0){memcpy(&gflist[lflists],nflist,nflists);}
	
	unsigned int mlisti=1;unsigned int mlistio=0;
	
	ma_engine eng;
	ma_engine_init(NULL,&eng);
	ma_decoder decoder={0};
	ma_sound sound={0};
	unsigned long long int mcp=0;

	unsigned char* mbuff=NULL;
	unsigned int sbuff=0;

	float cv=0.5;
	ma_engine_set_volume(&eng,cv);

	char idb=0;
	char lobg=0;
	char is_busy=0;
	char cur_opp=0;if (debug_struct.lstate==0 && debug_struct.nstate==0){cur_opp=-1;}

	while (run)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type==SDL_QUIT){run=0;}

			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)!=0 && ev.key.keysym.sym==SDLK_UP){if ((cv+0.05)<=1){cv+=0.05;ma_engine_set_volume(&eng,cv);break;}}
			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)!=0 && ev.key.keysym.sym==SDLK_DOWN){if ((cv-0.05)>=0){cv-=0.05;ma_engine_set_volume(&eng,cv);break;}}

			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)==0 && ev.key.keysym.sym==SDLK_UP){if (mlistio>0 && ((mlisti+2)*(14*font_size)==(3*(14*font_size)))){mlistio-=1;break;}if ((mlisti+mlistio)>1){mlisti-=1;break;}}
			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)==0 && ev.key.keysym.sym==SDLK_DOWN){if ((mlisti+mlistio)<tgmi && ((mlisti+1)*(14*font_size)<win_height)){mlisti+=1;break;}if ((mlisti+mlistio)<tgmi && ((mlisti+2)*(14*font_size)>=win_height)){mlistio+=1;break;}}
			
			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_d){if (idb==0){idb=1;break;}idb=0;}
			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_KP_PLUS){if (lobg>=0){break;}unsigned char* px;int pitch=0;if(SDL_LockTexture(btex,NULL,(void**)&px,&pitch)==0){for (unsigned int i=0;i<=tex_width*tex_height*3;i++){px[i]=px[i]*2;}SDL_UnlockTexture(btex);lobg+=1;}}
			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_KP_MINUS){if (lobg<=-5){break;}unsigned char* px;int pitch=0;if (SDL_LockTexture(btex,NULL,(void**)&px,&pitch)==0){for (unsigned int i=0;i<=tex_width*tex_height*3;i++){px[i]=px[i]/2;}SDL_UnlockTexture(btex);lobg-=1;}}
			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_SPACE &&ev.key.repeat==0){lply_posct(&sound);break;}
			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_RETURN &&ev.key.repeat==0 && is_busy==0){cur_opp=lply_capt(gmlist,gmlists,tgmi,tlmi,gflist,gflists,tgfi,mlisti,mlistio,&is_busy,conf.lmd,&mbuff,&sbuff,&eng,&decoder,&sound,&mcp,&sock,&faddr);}
		}
		if (cur_opp==-1){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);printl("Error: nstate=0 & lstate=0",2,255,0,0,ren,win_width/2-12*2*13,win_height/2-14*2,bff,bffs);SDL_RenderPresent(ren);}
		if (cur_opp==-2){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);printl("Error: file was not open",2,255,0,0,ren,win_width/2-12*2*12,win_height/2-14*2,bff,bffs);}
		
		if (cur_opp==0)
		{
			SDL_RenderCopy(ren,btex,NULL,NULL);

			lply_drawc(ren,font_size,win_width,win_height,mlisti-1,bff,bffs);

			/*for (unsigned int i=mlisti+mlistio-1;i<tgmi;i++)
			{
				if (ly>=(win_height-14*font_size)){break;}
				if (lx<(win_width-24*font_size)){printc(gmlist[i],font_size,conf.tc[0],conf.tc[1],conf.tc[2],ren,lx,ly,bff,bffs,0);lx+=12*font_size;}
				ly+=14*font_size;lx=0;
				printf("%d\n",i);
			}*/
			unsigned int tidx=0;unsigned int npv=0;for (unsigned int i=0;i<gmlists;i++){if (npv==mlistio){tidx=i;break;}if (gmlist[i]=='\n'){npv+=1;}}
			printl(&gmlist[tidx],1,conf.tc[0],conf.tc[1],conf.tc[2],ren,0,0,bff,bffs);
		}
		if (idb==1){SDL_RenderCopy(ren,btex,NULL,NULL);lply_debug(ren,win_width,bff,bffs,sfb,13,&debug_struct);printl(gmlist,1,conf.tc[0],conf.tc[1],conf.tc[2],ren,0,28,bff,bffs);lply_drawc(ren,font_size,win_width,win_height,mlisti+1,bff,bffs);}
		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}
	sclose(&sock);

	if (mbuff!=NULL){free(mbuff);}

	free(gmlist);
	free(gflist);

	free(nmlist);
	free(nflist);
	
	free(lmlist);
	free(lflist);
	
	free(bfbg);
	free(bff);
	
	ma_sound_uninit(&sound);
	ma_decoder_uninit(&decoder);
	ma_engine_uninit(&eng);

	SDL_DestroyTexture(btex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
