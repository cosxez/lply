#pragma once

void lply_sa(SDL_Renderer *ren,SDL_Texture *btex,unsigned char *bgid)
{
	unsigned char k=0;
	while (k<31)
	{
		unsigned char *px;
		int pitch;
		
		if (SDL_LockTexture(btex,NULL,(void**)(&px),&pitch)==0)
		{
			unsigned int ix=*(unsigned int*)bgid;
			unsigned int iy=*(unsigned int*)(bgid+4);
			for (unsigned int y=0;y<iy;y++)
			{
				for (unsigned int x=0;x<ix;x++)
				{
					px[(y*ix+x)*3]=bgid[(y*ix+x)*3+8]/30*k;
					px[(y*ix+x)*3+1]=bgid[(y*ix+x)*3+8+1]/30*k;
					px[(y*ix+x)*3+2]=bgid[(y*ix+x)*3+8+2]/30*k;
				}
			}
			SDL_UnlockTexture(btex);
		
			k+=1;
			SDL_RenderCopy(ren,btex,NULL,NULL);
			SDL_RenderPresent(ren);
			SDL_Delay(20);
		}
	}
}

void printc(char c,float font_size,unsigned char cr,unsigned char cg,unsigned cb,SDL_Renderer *ren,int x,int y,unsigned char* fd,unsigned int fcwqfs,unsigned short cmgc)
{
	if (cmgc==0)
	{
		unsigned int crp;
		crp=bcpitf[c];
		unsigned char lines = fd[crp];
		crp++;
			
		for (unsigned char l = 0; l < lines; l++)
		{
			unsigned char b1=fd[crp];
			unsigned char b2=fd[crp+1];

			crp += 2;
			int x1 = x + ((b1 >> 4) * font_size);
			int y1 = y + ((b1 & 0xf) * font_size);
			int x2 = x + ((b2 >> 4) * font_size);
			int y2 = y + ((b2 & 0xf) * font_size);
			SDL_SetRenderDrawColor(ren,cr,cg,cb,255);
			SDL_RenderDrawLine(ren,x1,y1,x2,y2);
		}
		return;
	}
	for (unsigned int crp=0;crp<fcwqfs;crp++)
	{
		if (crp+3>fcwqfs){break;}
		if (*(unsigned short*)(fd+crp) == cmgc)
		{
			crp += 2;
			unsigned char lines = fd[crp];
			crp++;
				
			for (unsigned char l = 0; l < lines; l++)
			{
				unsigned char b1=fd[crp];
				unsigned char b2=fd[crp+1];

				crp += 2;
				int x1 = x + ((b1 >> 4) * font_size);
				int y1 = y + ((b1 & 0xf) * font_size);
				int x2 = x + ((b2 >> 4) * font_size);
				int y2 = y + ((b2 & 0xf) * font_size);
				SDL_SetRenderDrawColor(ren,cr,cg,cb,255);
				SDL_RenderDrawLine(ren,x1,y1,x2,y2);
			}	
			return;
		}
	}
}

void printl(char* str,unsigned int strs,float font_size,unsigned char cr,unsigned char cg,unsigned char cb,SDL_Renderer* ren,int x,int y,unsigned char* fd,unsigned int fds)
{
	int sx=x;
	for (unsigned int i=0;i<strs;i++){if (str[i]=='\n'){y+=14*font_size;x=sx;continue;}printc(str[i],font_size,cr,cg,cb,ren,x,y,fd,fds,0);x+=12*font_size;}
}

void lply_debug(SDL_Renderer* ren,unsigned int win_width,unsigned char* bff,unsigned int bffs,unsigned char *sfb,unsigned int sfbs,struct dbs *des)
{
	for (unsigned int y=0;y<28;y++){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderDrawLine(ren,0,y,win_width,y);}
	printc('~',2,des->rstate==0 ? 255 : 0,des->rstate==1 ? 255 : 0,0,ren,win_width-win_width/100*24,0,sfb,sfbs,0xdd5a);
	printc('~',2,des->nstate==0 ? 255 : 0,des->nstate==1 ? 255 : 0,0,ren,win_width-win_width/100*20,0,sfb,sfbs,0x31f1);
	printl("ld",2,1,des->lstate==0 ? 255 : 0,des->lstate==1 ? 255 : 0,0,ren,win_width-win_width/100*16,13,bff,bffs);
}

void lply_drawc(char bluc,unsigned char cc[3],SDL_Renderer *ren,unsigned char font_size,int win_width,int win_height,unsigned int mlisti,unsigned char* bff,unsigned int bffs)
{
	if (bluc==1)
	{
		SDL_SetRenderDrawColor(ren,0,0,0,255);
		SDL_RenderFillRect(ren,&(SDL_Rect){win_width-24*font_size,0,win_width,win_height});
	}
	printc('<',1,cc[0],cc[1],cc[2],ren,win_width-24*font_size,mlisti*(14*font_size),bff,bffs,0);
	printc('-',1,cc[0],cc[1],cc[2],ren,win_width-12*font_size,mlisti*(14*font_size),bff,bffs,0);
}
