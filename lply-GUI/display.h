#pragma once

struct fcamn
{
	unsigned short mg;
	char ch;
}__attribute__((packed));

struct dbs
{
	char nstate;
	char lstate;
};

const struct fcamn fmcl[] =
{
    {0xB300, ' '}, {0xB301, '!'}, {0xB302, '"'}, {0xB303, '#'},
    {0xB304, '$'}, {0xB305, '%'}, {0xB306, '&'}, {0xB307, '\''},
    {0xB308, '('}, {0xB309, ')'}, {0xB30A, '*'}, {0xB30B, '+'},
    {0xB30C, ','}, {0xB30D, '-'}, {0xB30E, '.'}, {0xB30F, '/'},
    {0xB310, '0'}, {0xB311, '1'}, {0xB312, '2'}, {0xB313, '3'},
    {0xB314, '4'}, {0xB315, '5'}, {0xB316, '6'}, {0xB317, '7'},
    {0xB318, '8'}, {0xB319, '9'}, {0xB31A, ':'}, {0xB31B, ';'},
    {0xB31C, '<'}, {0xB31D, '='}, {0xB31E, '>'}, {0xB31F, '?'},
    {0xB320, '@'}, {0xB321, 'A'}, {0xB322, 'B'}, {0xB323, 'C'},
    {0xB324, 'D'}, {0xB325, 'E'}, {0xB326, 'F'}, {0xB327, 'G'},
    {0xB328, 'H'}, {0xB329, 'I'}, {0xB32A, 'J'}, {0xB32B, 'K'},
    {0xB32C, 'L'}, {0xB32D, 'M'}, {0xB32E, 'N'}, {0xB32F, 'O'},
    {0xB330, 'P'}, {0xB331, 'Q'}, {0xB332, 'R'}, {0xB333, 'S'},
    {0xB334, 'T'}, {0xB335, 'U'}, {0xB336, 'V'}, {0xB337, 'W'},
    {0xB338, 'X'}, {0xB339, 'Y'}, {0xB33A, 'Z'}, {0xB33B, '['},
    {0xB33C, '\\'}, {0xB33D, ']'}, {0xB33E, '^'}, {0xB33F, '_'},
    {0xB340, '`'}, {0xB341, 'a'}, {0xB342, 'b'}, {0xB343, 'c'},
    {0xB344, 'd'}, {0xB345, 'e'}, {0xB346, 'f'}, {0xB347, 'g'},
    {0xB348, 'h'}, {0xB349, 'i'}, {0xB34A, 'j'}, {0xB34B, 'k'},
    {0xB34C, 'l'}, {0xB34D, 'm'}, {0xB34E, 'n'}, {0xB34F, 'o'},
    {0xB350, 'p'}, {0xB351, 'q'}, {0xB352, 'r'}, {0xB353, 's'},
    {0xB354, 't'}, {0xB355, 'u'}, {0xB356, 'v'}, {0xB357, 'w'},
    {0xB358, 'x'}, {0xB359, 'y'}, {0xB35A, 'z'}, {0xB35B, '{'},
    {0xB35C, '|'}, {0xB35D, '}'}, {0xB35E, '~'}
};

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

void printc(char c,unsigned char font_size,unsigned char cr,unsigned char cg,unsigned cb,SDL_Renderer *ren,int x,int y,unsigned char* fd,unsigned int fcwqfs,unsigned short cmgc)
{
	unsigned short mgfc=0;
	if (cmgc==0)
	{
		for (unsigned int i = 0; i < sizeof(fmcl) / sizeof(fmcl[0]); i++)
		{
			if (c == fmcl[i].ch)
			{
				mgfc = fmcl[i].mg;break;
			}
		}
	}
	else{mgfc=cmgc;}
	if (mgfc==0){return;}
	
	for (unsigned int crp=0;crp<fcwqfs;crp++)
	{
		if (crp+3>fcwqfs){break;}
		if (*(unsigned short*)(fd+crp) == mgfc)
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

void printl(char* str,unsigned char font_size,unsigned char cr,unsigned char cg,unsigned char cb,SDL_Renderer* ren,int x,int y,unsigned char* fd,unsigned int fds)
{
	for (unsigned int i=0;str[i]!='\0';i++){if (str[i]=='\n'){y+=14*font_size;}printc(str[i],font_size,cr,cg,cb,ren,x,y,fd,fds,0);x+=12*font_size;}
}

void lply_debug(SDL_Renderer* ren,unsigned int win_width,unsigned char* bff,unsigned int bffs,unsigned char *sfb,unsigned int sfbs,struct dbs *des)
{
	for (unsigned int y=0;y<28;y++){SDL_SetRenderDrawColor(ren,0,0,0,255);SDL_RenderDrawLine(ren,0,y,win_width,y);}
	printc('~',2,des->nstate==0 ? 255 : 0,des->nstate==1 ? 255 : 0,0,ren,win_width-win_width/100*20,0,sfb,sfbs,0x31f1);
	printl("ld",1,des->lstate==0 ? 255 : 0,des->lstate==1 ? 255 : 0,0,ren,win_width-win_width/100*16,13,bff,bffs);
}
