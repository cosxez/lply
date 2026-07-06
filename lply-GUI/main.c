#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>
#include <math.h>

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
#include "thrds.h"

int main()
{
	struct cnf conf;conf.thie=0;conf.wm=0;memset(&(conf.tc),0,6);conf.win_width=864;conf.win_height=576;conf.drsbuff=32;conf.spblm=0;conf.bluc=0;conf.mfd=0;
	char cur_opp=0;
	if (cfg_pars(&conf)==1){cur_opp=-4;}
	
	int sock=-1;struct sockaddr_in addr;struct sockaddr_in faddr;memset(&addr,0,sizeof(addr));memset(&faddr,0,sizeof(faddr));
	
	struct dbs debug_struct;
	debug_struct.nstate=0;
	debug_struct.lstate=0;
	debug_struct.rstate=1;

	if (conf.wm==0)
	{
		if (lply_sbatt(&sock,&addr,&faddr,conf.ip,conf.port)!=0){sclose(&sock);debug_struct.nstate=0;}
		else{debug_struct.nstate=1;}
		if (lply_sbl(conf.lmd)!=0){debug_struct.lstate=0;}
		else{debug_struct.lstate=1;}
	}
	if (conf.wm==1){if (lply_sbatt(&sock,&addr,&faddr,conf.ip,conf.port)!=0){sclose(&sock);debug_struct.nstate=0;}else{debug_struct.nstate=1;}}
	if (conf.wm==2){if (lply_sbl(conf.lmd)!=0){debug_struct.lstate=0;}else{debug_struct.lstate=1;}}
	
	SDL_Window *win=SDL_CreateWindow("lply",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,conf.win_width,conf.win_height,SDL_WINDOW_SHOWN);
	SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);

	char run=1;
	SDL_Event ev;
	
	unsigned char *bfbg=NULL;
	unsigned char *bff=NULL;
	unsigned int bffs;
	if (lply_sthm(conf.bgi,conf.thie,&bfbg,&bff,&bffs)!=0){return -2;}
	SDL_Texture *btex=NULL;btex=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,*(unsigned int*)bfbg,*(unsigned int*)(bfbg+4));unsigned int tex_width=*(unsigned int*)bfbg;unsigned int tex_height=*(unsigned int*)(bfbg+4);
	lply_sa(ren,btex,bfbg);printl("loading...",10,1,144,121,255,ren,conf.win_width/2-12*5,conf.win_height/2,bff,bffs);SDL_RenderPresent(ren);
	
	unsigned char sfb[]={0xf1,0x31,0x05,0x7b,0x8b,0x58,0x87,0x87,0xa8,0x85,0x46,0x85,0xb6,0x5a,0xdd,0x0c,0x26,0xd6,0x26,0x2c,0x2c,0xdc,0xdc,0xd6,0x48,0x4a,0x48,0x68,0x68,0x6a,0x6a,0x4a,0x8a,0xaa,0x8a,0x88,0x88,0xa8,0xa8,0xaa};
	unsigned char font_size=1;

	char *nmlist=NULL;unsigned int nmlists=0;unsigned int tnmi=0;
	char *nflist=NULL;unsigned int nflists=0;unsigned int tnfi=0;
	if (debug_struct.nstate==1){if (lply_gmlfls(&sock,&faddr,&nmlist,&nmlists,&tnmi,&nflist,&nflists,&tnfi)!=0){debug_struct.nstate=0;}}

	char *lmlist=NULL;unsigned int lmlists=0;unsigned int tlmi=0;
	char *lflist=NULL;unsigned int lflists=0;unsigned int tlfi=0;
	if (debug_struct.lstate==1){lply_gmlfld(&lmlist,&lmlists,&tlmi,&lflist,&lflists,&tlfi,conf.lmd);}
	
	char *gmlist=NULL;unsigned int gmlists=0;unsigned int tgmi=0;gmlists=lmlists+nmlists;tgmi=tlmi+tnmi;gmlist=(char*)malloc(gmlists+1);if (lmlist!=NULL && lmlists>0){memcpy(gmlist,lmlist,lmlists);}if (nmlist!=NULL && nmlists>0){memcpy(&gmlist[lmlists],nmlist,nmlists);}
	char *gflist=NULL;unsigned int gflists=0;unsigned int tgfi=0;gflists=lflists+nflists;tgfi=tlfi+tnfi;gflist=(char*)malloc(gflists+1);if (lflist!=NULL && lflists>0){memcpy(gflist,lflist,lflists);}if (nflist!=NULL && nflists>0){memcpy(&gflist[lflists],nflist,nflists);}
	
	unsigned int mlisti=1;unsigned int mlistio=0;unsigned int cidx=0;
	
	ma_engine eng;
	ma_engine_init(NULL,&eng);
	ma_decoder decoder={0};
	ma_sound sound={0};
	unsigned long long int mcp=0;

	unsigned char* mbuff=NULL;
	unsigned int sbuff=1024*1024*3;unsigned int rmds=0;
	mbuff=(unsigned char*)malloc(sbuff);

	unsigned int ncrp=0;

	SRAMm* rmbuff=(SRAMm*)malloc(conf.drsbuff*sizeof(SRAMm));unsigned short cua=0;
	unsigned short rmi=1;unsigned short rmio=0;

	float cv=0.5;
	ma_engine_set_volume(&eng,cv);

	char is_busy=0;
	char lobg=0;
	if (debug_struct.lstate==0 && debug_struct.nstate==0 && cur_opp==0){cur_opp=-1;}

	while (run)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type==SDL_QUIT){run=0;}

			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)!=0 && ev.key.keysym.sym==SDLK_UP){if ((cv+0.05)<=1){cv+=0.05;ma_engine_set_volume(&eng,cv);break;}}
			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)!=0 && ev.key.keysym.sym==SDLK_DOWN){if ((cv-0.05)>=0){cv-=0.05;ma_engine_set_volume(&eng,cv);break;}}
			
			if (cur_opp==2 &&ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)==0 && ev.key.keysym.sym==SDLK_UP){if (rmio>0 && ((rmi+2)*(14*font_size)==(3*(14*font_size)))){rmio-=1;break;}if ((rmi+rmio)>1){rmi-=1;break;}}
			if (cur_opp==2 &&ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)==0 && ev.key.keysym.sym==SDLK_DOWN){if ((rmi+rmio)<cua && ((rmi+2)*(14*font_size)<conf.win_height)){rmi+=1;break;}if ((rmi+rmio)<tgmi && ((rmi+2)*(14*font_size)>=conf.win_height)){rmio+=1;break;}}

			if (cur_opp==0 &&ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)==0 && ev.key.keysym.sym==SDLK_UP){if (mlistio>0 && ((mlisti+2)*(14*font_size)==(3*(14*font_size)))){mlistio-=1;break;}if ((mlisti+mlistio)>1){mlisti-=1;break;}}
			if (cur_opp==0 &&ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)==0 && ev.key.keysym.sym==SDLK_DOWN){if ((mlisti+mlistio)<tgmi && ((mlisti+2)*(14*font_size)<conf.win_height)){mlisti+=1;break;}if ((mlisti+mlistio)<tgmi && ((mlisti+2)*(14*font_size)>=conf.win_height)){mlistio+=1;break;}}
			
			if (cur_opp==0 &&is_busy==0 &&ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_p){char ign=0;for (unsigned short i=0;i<cua;i++){if (rmbuff[i].idx==cidx){ign=1;break;}}if (ign==0&&cua<512){is_busy=1;SRAMm ts;ts.mba=(unsigned char*)malloc(rmds);memcpy(ts.mba,mbuff,rmds);ts.mbs=rmds;unsigned int npv=0;unsigned int tidx=0;for (unsigned int i=0;i<gmlists;i++){if (npv==cidx-1){tidx=i;break;}if (gmlist[i]=='\n'){npv+=1;}}for (unsigned int i=tidx;i<gmlists+1;i++){if (npv==cidx){ts.mn=(char*)malloc(i-tidx);memcpy(ts.mn,&gmlist[tidx],i-tidx);ts.mns=i-tidx;break;}if (gmlist[i]=='\n'){npv+=1;}}ts.idx=cidx;rmbuff[cua]=ts;cua+=1;is_busy=0;}break;}

			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_d){if (cur_opp==0){cur_opp=1;break;}cur_opp=0;}

			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_r){if (cur_opp==0){cur_opp=2;break;}cur_opp=0;}

			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_KP_PLUS){if (lobg>=0){break;}unsigned char* px;int pitch=0;if(SDL_LockTexture(btex,NULL,(void**)&px,&pitch)==0){for (unsigned int i=0;i<=tex_width*tex_height*3;i++){px[i]=px[i]*2;}SDL_UnlockTexture(btex);lobg+=1;}}
			if (ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_KP_MINUS){if (lobg<=-5){break;}unsigned char* px;int pitch=0;if (SDL_LockTexture(btex,NULL,(void**)&px,&pitch)==0){for (unsigned int i=0;i<=tex_width*tex_height*3;i++){px[i]=px[i]/2;}SDL_UnlockTexture(btex);lobg-=1;}}
			if (is_busy==0 &&ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_SPACE &&ev.key.repeat==0){lply_posct(&sound);break;}
			if (cur_opp==0 &&is_busy==0 &&ev.type==SDL_KEYDOWN &&ev.key.keysym.sym==SDLK_RETURN &&ev.key.repeat==0){cidx=mlisti+mlistio;thrd_t thr;TSlply_capt* ts=malloc(sizeof(TSlply_capt));ts->rmb=rmbuff;ts->rmbs=cua;ts->mlist=gmlist;ts->mlistrm=gmlists;ts->tgmi=tgmi;ts->tlmi=tlmi;ts->flist=gflist;ts->flists=gflists;ts->tgfi=tgfi;ts->mlisti=mlisti;ts->mlistio=mlistio;ts->is_busy=&is_busy;ts->ld=conf.lmd;ts->mbuff=&mbuff;ts->sbuff=&sbuff;ts->mds=&rmds;ts->eng=&eng;ts->decoder=&decoder;ts->sound=&sound;ts->mcp=&mcp;ts->sock=&sock;ts->faddr=&faddr;ts->ra=&cur_opp;ts->nls=&ncrp;ts->mfd=conf.mfd;thrd_create(&thr,lply_tcapt,ts);thrd_detach(thr);}
			if (cur_opp==2 &&is_busy==0 &&ev.type==SDL_KEYDOWN&&ev.key.keysym.sym==SDLK_RETURN &&ev.key.repeat==0){thrd_t thr;TSlply_ptfr* ts=malloc(sizeof(TSlply_ptfr));ts->rmb=rmbuff;ts->rmbs=cua;ts->tidx=rmi+rmio-1;ts->mbuff=&mbuff;ts->sbuff=&sbuff;ts->mds=&rmds;ts->mcp=&mcp;ts->eng=&eng;ts->sound=&sound;ts->decoder=&decoder;ts->is_busy=&is_busy;thrd_create(&thr,lply_tptfr,ts);thrd_detach(thr);}
		}
		if (cur_opp==1){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);lply_debug(ren,conf.win_width,bff,bffs,sfb,sizeof(sfb),&debug_struct);}
		if (cur_opp==-1){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);printl("Error: nstate=0 & lstate=0",26,2,255,0,0,ren,conf.win_width/2-12*2*13,conf.win_height/2-14*2,bff,bffs);}
		if (cur_opp==-2){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);printl("Error: file was not open",24,2,255,0,0,ren,conf.win_width/2-12*2*12,conf.win_height/2-14*2,bff,bffs);}
		if (cur_opp==-3){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);printl("Error: failed to get media data size",36,2,255,0,0,ren,conf.win_width/2-12*2*18,conf.win_height/2-14*2,bff,bffs);}
		if (cur_opp==-4){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderClear(ren);printl("Error: file \"config\" not found",30,2,255,0,0,ren,conf.win_width/2-12*2*15,conf.win_height/2-14*2,bff,bffs);}

		if (cur_opp==0)
		{
			SDL_RenderCopy(ren,btex,NULL,NULL);

			lply_drawc(conf.bluc,conf.cc,ren,font_size,conf.win_width,conf.win_height,mlisti,bff,bffs);

			unsigned int tidx=0;unsigned int tidxfct=0;unsigned int npv=0;for (unsigned int i=0;i<gmlists;i++){if (npv==mlistio){tidx=i;break;}if (gmlist[i]=='\n'){npv+=1;}}npv=0;for (unsigned int i=0;i<gmlists;i++){if (npv==mlisti+mlistio-1){tidxfct=i;break;}if (gmlist[i]=='\n'){npv+=1;}}
			unsigned int lx=0;
			for (unsigned int i=tidxfct;gmlist[i]!='\n';i++){printc(gmlist[i],font_size,conf.tc[0],conf.tc[1],conf.tc[2],ren,lx,0,bff,bffs,0);lx+=12*font_size;}
			printl(&gmlist[tidx],gmlists,font_size,conf.tc[0],conf.tc[1],conf.tc[2],ren,0,14*font_size,bff,bffs);
			if (is_busy==1 && conf.spblm==1){SDL_SetRenderDrawColor(ren,conf.tc[0],conf.tc[1],conf.tc[2],255);SDL_RenderDrawLine(ren,conf.win_width/2,14*font_size,conf.win_width/2+(ncrp/(sbuff/(conf.win_width/100*20))),14*font_size);}
		}
		if (cur_opp==2)
		{
			SDL_RenderCopy(ren,btex,NULL,NULL);

			lply_drawc(conf.bluc,conf.cc,ren,font_size,conf.win_width,conf.win_height,rmi,bff,bffs);
			
			for (unsigned short i=rmio;i<cua;i++){printl(rmbuff[i].mn,rmbuff[i].mns,font_size,conf.tc[0],conf.tc[1],conf.tc[2],ren,0,(i-rmio+1)*14*font_size,bff,bffs);}
			printl(rmbuff[rmi+rmio-1].mn,rmbuff[rmi+rmio-1].mns,font_size,conf.tc[0],conf.tc[1],conf.tc[2],ren,0,0,bff,bffs);
		}
		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}
	sclose(&sock);

	if (ma_sound_is_playing(&sound)){ma_sound_stop(&sound);}
	
	for (unsigned short i=0;i<cua;i++){if (rmbuff[i].mba!=NULL){free(rmbuff[i].mba);rmbuff[i].mba=NULL;}if (rmbuff[i].mn!=NULL){free(rmbuff[i].mn);rmbuff[i].mn=NULL;}}

	if (rmbuff!=NULL){free(rmbuff);}
	
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
