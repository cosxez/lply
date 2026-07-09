#pragma once

char cfg_pars(struct cnf* conf)
{
	FILE *file=fopen("config","r");
	if (file!=NULL)
	{
		char *cl=NULL;
		size_t cls=0;
		size_t prs=0;

		char arg[128];char pr[1024];
		
		int rss=0;
		while (rss!=-1)
		{
			rss=getline(&cl,&cls,file);
			if (rss==-1){break;}
			for (int i=0;i<rss;i++)
			{
				if (i<128 &&(cl[i]==' ' || cl[i]=='='))
				{
					memcpy(arg,cl,i);arg[i]='\0';
					break;
				}
			}
			for (int i=rss-1;i>0;i--)
			{
				if (i<1024&&(cl[i]==' ' || cl[i]=='='))
				{
					memcpy(pr,cl+i+1,rss-i-1);
					prs=rss-i-2;pr[prs]='\0';
					break;
				}
			}
			if (arg==NULL||pr==NULL){continue;}
			if (strcmp(arg,"ip")==0)
			{
				memcpy(conf->ip,pr,prs);
				conf->ip[prs]='\0';
				continue;
			}
			if (strcmp(arg,"port")==0)
			{
				conf->port=(unsigned short)atoi(pr);continue;
			}
			if (strcmp(arg,"theme-file")==0)
			{
				memcpy(conf->bgi,pr,prs);
				conf->bgi[prs]='\0';
				conf->thie=1;
				continue;
			}
			if (strcmp(arg,"lm-dir")==0)
			{
				memcpy(conf->lmd,pr,prs);
				if (pr[prs-1]=='/'){conf->lmd[prs]='\0';continue;}
				conf->lmd[prs]='/';conf->lmd[prs+1]='\0';
			}
			if (strcmp(arg,"work-mode")==0)
			{
				if (strcmp(pr,"no")==0){conf->wm=1;}
				if (strcmp(pr,"lo")==0){conf->wm=2;}
				if (strcmp(pr,"uv")==0){conf->wm=0;}
				continue;
			}
			if (strcmp(arg,"text-color")==0)
			{
				unsigned char tidx=0;unsigned char pb=0;for (unsigned char i=0;i<prs+1;i++){if (pr[i]==','||i==prs){char crc[i-tidx+1];memcpy(crc,&pr[tidx],i-tidx);crc[i-tidx]='\0';conf->tc[pb]=(unsigned char)atoi(crc);pb+=1;tidx=i+1;}}continue;
			}
			if (strcmp(arg,"cursor-color")==0)
			{
				unsigned char tidx=0;unsigned char pb=0;for (unsigned char i=0;i<prs+1;i++){if (pr[i]==','||i==prs){char crc[i-tidx+1];memcpy(crc,&pr[tidx],i-tidx);crc[i-tidx]='\0';conf->cc[pb]=(unsigned char)atoi(crc);pb+=1;tidx=i+1;}}continue;
			}
			if (strcmp(arg,"black-line-under-cursor")==0)
			{
				if (strcmp(pr,"y")==0){conf->bluc=1;}continue;
			}
			if (strcmp(arg,"win-width")==0)
			{
				conf->win_width=(int)atoi(pr);continue;
			}
			if (strcmp(arg,"win-height")==0)
			{
				conf->win_height=(int)atoi(pr);continue;
			}
			if (strcmp(arg,"reserve-ram")==0)
			{
				conf->drsbuff=(unsigned int)atoi(pr);continue;
			}
			if (strcmp(arg,"show-progress-bar")==0)
			{
				if (strcmp(pr,"y")==0){conf->spblm=1;continue;}
			}
			if (strcmp(arg,"download-method")==0)
			{
				if (strcmp(pr,"parallel")==0){conf->mfd=0;continue;}
				if (strcmp(pr,"sequential")==0){conf->mfd=1;continue;}
			}
		}
		free(cl);
		fclose(file);
		return 0;
	}
	return 1;
}
