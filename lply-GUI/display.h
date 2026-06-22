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
