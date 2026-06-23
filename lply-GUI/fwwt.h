#pragma once

char lply_sthm(char *bgi,char thie,unsigned char **bfbg,unsigned char **bff,unsigned int *bffs)
{
	if (thie==1)
	{
		FILE *file=fopen(bgi,"r");
		if (file==NULL){return -1;}

		unsigned int ids;fread(&ids,4,1,file);
		*bfbg=(char*)malloc(ids);fread(*bfbg,ids,1,file);
		
		unsigned int fds;fread(&fds,4,1,file);*bffs=fds;
		*bff=(char*)malloc(fds);fread(*bff,fds,1,file);
		fclose(file);
		return 0;
	}
	FILE *file=fopen("base-thm.jdft","r");
	if (file==NULL){return -1;}

	unsigned int ids;fread(&ids,4,1,file);
	*bfbg=(char*)malloc(ids);fread(*bfbg,ids,1,file);

	unsigned int fds;fread(&fds,4,1,file);*bffs=fds;
	*bff=(char*)malloc(fds);fread(*bff,fds,1,file);
	fclose(file);
	return 0;
}
