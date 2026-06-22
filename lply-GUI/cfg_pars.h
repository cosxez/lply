#pragma once

struct cnf
{
	char bgi[511];
	char ip[128];
	unsigned short port;
	char thie;
	char wm;
};

char cfg_pars(struct cnf* conf)
{
	FILE *file=fopen("config","r");
	if (file!=NULL)
	{
		char *cl=NULL;
		char *arg=NULL;
		char *pr=NULL;
		size_t cls=0;
		size_t prs=0;
		
		int rss=0;
		while (rss!=-1)
		{
			rss=getline(&cl,&cls,file);
			if (rss==-1){break;}
			for (int i=0;i<rss;i++)
			{
				if (cl[i]==' ' || cl[i]=='=')
				{
					arg=(char*)malloc(i);
					memcpy(arg,cl,i);arg[i]='\0';
					break;
				}
			}
			for (int i=rss-1;i>0;i--)
			{
				if (cl[i]==' ' || cl[i]=='=')
				{
					pr=(char*)malloc(rss-i-2);
					memcpy(pr,cl+i+1,rss-i-2);
					prs=rss-i-2;
					break;
				}
			}
			if (strcmp(arg,"ip")==0)
			{
				memcpy(conf->ip,pr,prs);
				conf->ip[prs]='\0';
			}
			if (strcmp(arg,"port")==0)
			{
				conf->port=atoi(pr);
			}
			if (strcmp(arg,"theme-file")==0)
			{
				memcpy(conf->bgi,pr,prs);
				conf->bgi[prs]='\0';
				conf->thie=1;
			}
			if (strcmp(arg,""))
			free(pr);
			free(arg);
		}
		free(cl);
		fclose(file);
		return 0;
	}
	return 1;
}
