#pragma once

char lply_sthm(struct cnf *conf,unsigned char **bfbg,unsigned char **bff)
{
	if (conf->thie==1)
	{
		FILE *file=fopen(conf->bgi,"r");
		if (file==NULL){return -1;}

		unsigned int ids;fread(&ids,4,1,file);
		*bfbg=(unsigned char*)malloc(ids);fread(*bfbg,ids,1,file);
		
		unsigned int fds;fread(&fds,4,1,file);
		*bff=(unsigned char*)malloc(fds);fread(*bff,fds,1,file);
		fclose(file);
		return 0;
	}
	FILE *file=fopen("base-thm.jdft","r");
	if (file==NULL){return -1;}

	unsigned int ids;fread(&ids,4,1,file);
	*bfbg=(unsigned char*)malloc(ids);fread(*bfbg,ids,1,file);

	unsigned int fds;fread(&fds,4,1,file);
	*bff=(unsigned char*)malloc(fds);fread(*bff,fds,1,file);
	fclose(file);
	return 0;
}
