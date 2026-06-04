#define MINIAUDIO_IMPLEMENTATION

#define END_NEXT 0
#define END_REPLAY 1
#define END_STOP 2

#include "miniaudio.h"
#include <SDL2/SDL.h>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "cfg_pars.h"
#include "sdh.h"
#include "network.h"
#include "font.h"
#include "sound.h"

int main(int args,char* argv[])
{	
	std::string ip;
	unsigned short port;
	struct cnf conf;

	if (cfg_pars(&conf,&ip,&port)!=0){return -1;}

	int sock=socket(AF_INET,SOCK_DGRAM,0);
	
	struct sockaddr_in addr;
	struct sockaddr_in faddr;
	
	if (lply_sbatt(&sock,&addr,&faddr,ip,port)!=0){sclose(&sock);return -1;}
	
	unsigned short mgcfgml=0xa1b3;
	sendto(sock,&mgcfgml,sizeof(mgcfgml),0,(struct sockaddr*)&faddr,sizeof(faddr));

	size_t mlistrm;
	if (lply_read(&sock,&faddr,&mlistrm,sizeof(mlistrm))<1){sclose(&sock);return -2;}
	if (mlistrm==0){return -322;}
	char mlist[mlistrm];
	if (lply_read(&sock,&faddr,&mlist,mlistrm)<1){sclose(&sock);return -2;}

	size_t flistrm;
	if (lply_read(&sock,&faddr,&flistrm,sizeof(flistrm))<1){sclose(&sock);return -2;}
	if (flistrm==0){return -332;}
	char flist[flistrm];
	if (lply_read(&sock,&faddr,&flist,flistrm)<1){sclose(&sock);return -2;}

	std::ifstream bgbf(argv[0],std::ios::binary);
	if (!bgbf.is_open()){sclose(&sock);return -3;}
	bgbf.seekg(0,std::ios::end);unsigned int bgbffs=bgbf.tellg();
	bgbf.seekg(bgbffs-4,std::ios::beg);
	bgbf.read(reinterpret_cast<char*>(&fcwqfs),sizeof(fcwqfs));bgbf.seekg(bgbffs-4-fcwqfs,std::ios::beg);
	_binary_font_cwqf_start=new unsigned char[fcwqfs];bgbf.read(reinterpret_cast<char*>(_binary_font_cwqf_start),fcwqfs);

	bgbf.seekg(bgbffs-8-fcwqfs,std::ios::beg);
	unsigned int bgbfs;bgbf.read(reinterpret_cast<char*>(&bgbfs),sizeof(bgbfs));bgbf.seekg(bgbffs-8-bgbfs-fcwqfs,std::ios::beg);
	unsigned char _binary_cvnt_whld_start[bgbfs];bgbf.read(reinterpret_cast<char*>(_binary_cvnt_whld_start),bgbfs);
	bgbf.close();

	int win_width=720;
	int win_height=480;
	
	SDL_Window *win=SDL_CreateWindow("Lain player",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,win_width,win_height,SDL_WINDOW_SHOWN);
	SDL_Renderer *ren=SDL_CreateRenderer(win,-1,0);

	char run=1;
	SDL_Event ev;
	
	SDL_Texture *btex=nullptr;
	btex=SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGB24,SDL_TEXTUREACCESS_STREAMING,win_width,win_height);

	unsigned char k=0;
	while (k<11)
	{
		unsigned char *px;
		int pitch;

		if (SDL_LockTexture(btex,NULL,(void**)(&px),&pitch)==0)
		{
			for (unsigned int y=0;y<win_height;y++)
			{
				for (unsigned int x=0;x<win_width;x++)
				{
					px[(y*win_width+x)*3]=_binary_cvnt_whld_start[(y*win_width+x)*3+8]/10*k;
					px[(y*win_width+x)*3+1]=_binary_cvnt_whld_start[(y*win_width+x)*3+8+1]/10*k;
					px[(y*win_width+x)*3+2]=_binary_cvnt_whld_start[(y*win_width+x)*3+8+2]/10*k;
				}
			}
			SDL_UnlockTexture(btex);
		}
		k+=1;
		SDL_RenderCopy(ren,btex,NULL,NULL);
		SDL_RenderPresent(ren);
		SDL_Delay(20);
	}
	
	unsigned char font_size=1;
	unsigned int lx=0;unsigned int ly=14*font_size;
	unsigned int mlisti=1;
	unsigned int mlistio=0;
	
	std::vector<std::string> mvec;
	{
		std::string sfmvec;
		for (unsigned int i=0;i<mlistrm;i++)
		{
			if (mlist[i]=='\n' || i>=mlistrm){mvec.push_back(sfmvec);sfmvec="";i+=1;}
			sfmvec+=mlist[i];
		}
	}

	std::vector<std::vector<unsigned char>> tmv;
//	std::vector<unsigned char> mbuff;

	unsigned long long int mcp=0;
	char* mbuff=nullptr;

	ma_engine eng;
	ma_engine_init(NULL,&eng);

	ma_decoder decoder={0};
	ma_sound sound={0};

	float cv=0.5;
	ma_engine_set_volume(&eng,cv);
	char is_busy=0;

	char l_mode=END_REPLAY;
	while (run)
	{
		SDL_RenderCopy(ren,btex,NULL,NULL);
		SDL_SetRenderDrawColor(ren,0,0,0,255);
		for (unsigned int y=14*font_size;y<win_height;y++){SDL_RenderDrawLine(ren,win_width-24*font_size,y,win_width,y);}
		for (unsigned short i=0;i<mvec[mlisti+mlistio-1].size();i++){printc(mvec[mlisti+mlistio-1][i],font_size,0,0,0,ren,lx,0);lx+=12*font_size;}
		
		lx=0;ly=14*font_size;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type==SDL_QUIT){run=0;}
			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)!=0 && ev.key.keysym.sym==SDLK_UP){if ((cv+0.05)<=1){cv+=0.05;ma_engine_set_volume(&eng,cv);break;}}
			if (ev.type==SDL_KEYDOWN && (ev.key.keysym.mod & KMOD_ALT)!=0 && ev.key.keysym.sym==SDLK_DOWN){if ((cv-0.05)>=0){cv-=0.05;ma_engine_set_volume(&eng,cv);break;}}

			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_UP){if (mlistio>0 && ((mlisti+2)*(14*font_size)==(3*(14*font_size)))){mlistio-=1;break;}if ((mlisti+mlistio)>1){mlisti-=1;break;}}
			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_DOWN){if ((mlisti+mlistio)<mvec.size() && ((mlisti+2)*(14*font_size)<win_height)){mlisti+=1;break;}if ((mlisti+mlistio)<mvec.size() && ((mlisti+2)*(14*font_size)>=win_height)){mlistio+=1;break;}}
			
			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_RIGHT){lply_mttit(&eng,&sound,5,mcp,l_mode);}
			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_LEFT){lply_mttit(&eng,&sound,-5,mcp);}
			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_SPACE){if (ma_sound_is_playing(&sound)){ma_sound_stop(&sound);break;}if (!ma_sound_is_playing(&sound)){ma_sound_start(&sound);break;}}
			if (ev.type==SDL_KEYDOWN && ev.key.keysym.sym==SDLK_RETURN && ev.key.repeat==0)
			{
				unsigned int smli=0;
				std::string sfmi;
				for (unsigned int i=0;i<mlistrm;i++)
				{
					if (mlist[i]=='\n'){smli+=1;if (smli==(mlisti+mlistio)){break;}sfmi="";i+=1;}
					sfmi+=mlist[i];
				}

				char cirm=0;
				unsigned short tmvi=0;
				for (tmvi=0;tmvi<tmv.size();tmvi++)
				{
					unsigned short sns;memcpy(&sns,tmv[tmvi].data(),sizeof(sns));
					char tbfch[sns];
					if (sfmi==tbfch){cirm=1;break;}
				}
				
				if (cirm==0 && is_busy==0)
				{
					unsigned int sfli=0;
					std::string sfb;
					for (unsigned int i=0;i<flistrm;i++)
					{
						if (flist[i]=='\n'){sfli+=1;if (sfli==smli){break;}sfb="";i+=1;}
						sfb+=flist[i];
					}

					unsigned char bfgmd[2+sfb.size()];
					*(unsigned short*)bfgmd=0x65f1;
					memcpy(&bfgmd[2],sfb.data(),sfb.size());

					//sendto(sock,&bfgmd,sizeof(bfgmd),0,(struct sockaddr*)&faddr,sizeof(faddr));
					size_t bsfm;
					for (unsigned char i=0;i<32;i++)
					{
						sendto(sock,&bfgmd,sizeof(bfgmd),0,(struct sockaddr*)&faddr,sizeof(faddr));
						std::this_thread::sleep_for(std::chrono::milliseconds(500));
						char tbuff[10];
						if (lply_read(&sock,&faddr,&tbuff,sizeof(tbuff),MSG_DONTWAIT)==10 && *(unsigned short*)tbuff==0xa5f1){memcpy(&bsfm,&tbuff[2],8);break;}
						if (i==31){return -322;}
					}
					mbuff=(char*)malloc(bsfm);	
				//	char* tmp=(char*)realloc(mbuff,bsfm);
				//	mbuff=tmp;mbuffs=bsfm;
				//	mbuff.resize(bsfm);
					
					if (conf.with_s='y'){std::thread(lply_rmdaps,&sock,&faddr,mbuff,bsfm,&eng,&decoder,&sound,&is_busy,sfb,&mcp).detach();}
					else{std::thread(lply_rmdap,&sock,&faddr,mbuff,bsfm,&eng,&decoder,&sound,&is_busy,&mcp).detach();}
					
					continue;
				}
				if (cirm==1)
				{
					ma_decoder_init_memory(tmv[tmvi].data(), tmv[tmvi].size(), NULL, &decoder);
					ma_sound_init_from_data_source(&eng,&decoder,0,NULL,&sound);
				}
			}
		}
		for (unsigned int i=mlistio;i<mvec.size();i++)
		{
			if (ly>=(win_height-14*font_size)){break;}
			for (unsigned int j=0;j<mvec[i].size();j++)
			{
				if (lx<(win_width-24*font_size)){printc(mvec[i][j],font_size,0,0,0,ren,lx,ly);lx+=12*font_size;}
			}
			ly+=14*font_size;lx=0;
		}
		printc('<',font_size,255,255,255,ren,win_width-24*font_size,mlisti*(14*font_size));printc('-',font_size,255,255,255,ren,win_width-12*font_size,mlisti*(14*font_size));

		SDL_RenderPresent(ren);
		SDL_Delay(20);
		lx=0;ly=14*font_size;
	}
	sclose(&sock);
	
	ma_sound_uninit(&sound);
	ma_decoder_uninit(&decoder);
	ma_engine_uninit(&eng);
	
	free(mbuff);
	delete[] _binary_font_cwqf_start;
	SDL_DestroyTexture(btex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
