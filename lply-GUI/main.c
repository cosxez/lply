#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "cfg_pars.h"
#include "fwwt.h"
#include "display.h"
#include "network.h"
#include "sound.h"
#include "lmdr.h"

int main()
{
	struct cnf conf;conf.thie=0;conf.wm=0;
	if (cfg_pars(&conf)!=0){return -1;}
	
	int sock=-1;struct sockaddr_in addr;struct sockaddr_in faddr;
	
	char nstate=0;
	char lstate=0;
	if (conf.wm==0)
	{
		if (lply_sbatt(&sock,&addr,&faddr,conf.ip,conf.port)!=0){sclose(&sock);nstate=0;}
		else{nstate=1;}
		if (lply_sbl(conf.lmd)!=0){lstate=0;}
		else{lstate=1;}
	}
	if (conf.wm==1){if (lply_sbatt(&sock,&addr,&faddr,conf.ip,conf.port)!=0){sclose(&sock);nstate=0;}else{nstate=1;}}
	if (conf.wm==2){if (lply_sbl(conf.lmd)!=0){lstate=0;}else{lstate=1;}}
	
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
	SDL_Texture *btex=NULL;btex=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,*(unsigned int*)bfbg,*(unsigned int*)(bfbg+4));
	lply_sa(ren,btex,bfbg);
	
	while (run)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type==SDL_QUIT){run=0;}
		}
		SDL_RenderCopy(ren,btex,NULL,NULL);
		
		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}
	sclose(&sock);
	free(bfbg);
	free(bff);
	SDL_DestroyTexture(btex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
