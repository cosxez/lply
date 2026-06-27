#pragma once

char lply_sbl(char* path)
{
	DIR *dir=opendir(path);
	if (dir==NULL){return -1;}
	closedir(dir);
	return 0;
}

char lply_gmdfld(unsigned char **mbuff,unsigned int *sbuff,char *ld,char *fn)
{
	char pathff[512];
	snprintf(pathff,511,"%s%s",ld,fn);
	
	FILE *file=fopen(pathff,"r");
	if (file==NULL){return -1;}
	
	fseek(file,0,SEEK_END);unsigned int fs=ftell(file);fseek(file,0,SEEK_SET);
	if (*mbuff!=NULL){free(*mbuff);*mbuff=NULL;}
	*mbuff=(char*)malloc(fs);fread(*mbuff,fs,1,file);

	fclose(file);
	*sbuff=fs;
	return 0;
}

void lply_gmlfld(char **str,unsigned int *strs,unsigned int *stri,char **fn,unsigned int *fs,unsigned int *fi,char* path)
{
	DIR *dir=opendir(path);
	if (dir==NULL){return;}
	FILE *file=NULL;

	struct dirent *cf;
	
	char pathff[512];

	unsigned int fns=0;
	unsigned int sns=0;
	while ((cf=readdir(dir))!=NULL)
	{
		if (strcmp(cf->d_name,".")==0 || strcmp(cf->d_name,"..")==0){continue;}

		snprintf(pathff,511,"%s%s",path,cf->d_name);
		file=fopen(pathff,"r");
		if (file==NULL){continue;}

		unsigned int mgc;fread(&mgc,4,1,file);
		if (mgc==0xe0f1a4b3)
		{
			unsigned short fsns;
			fread(&fsns,2,1,file);sns+=fsns+1;
		}
		else{sns+=strlen(cf->d_name)+1;}
		fclose(file);file=NULL;
		
		fns+=strlen(cf->d_name)+1;

		*stri+=1;*fi+=1;
	}
	rewinddir(dir);

	*fn=(char*)malloc(fns+1);
	*str=(char*)malloc(sns+1);

	unsigned int cp=0;
	unsigned int ncp=0;
	while ((cf=readdir(dir))!=NULL)
	{
		if (strcmp(cf->d_name,".")==0 || strcmp(cf->d_name,"..")==0){continue;}
		
		snprintf(pathff,511,"%s%s",path,cf->d_name);
		file=fopen(pathff,"r");
		if (file==NULL){continue;}
		
		unsigned int mgc;fread(&mgc,4,1,file);
		if (mgc==0xe0f1a4b3)
		{
			unsigned short fsns;
			fread(&fsns,2,1,file);fread(&((*str)[ncp]),fsns,1,file);ncp+=fsns;(*str)[ncp]='\n';ncp+=1;
		}
		else{memcpy(&(*str)[ncp],cf->d_name,strlen(cf->d_name));ncp+=strlen(cf->d_name);(*str)[ncp]='\n';ncp+=1;}
		fclose(file);file=NULL;

		memcpy(&(*fn)[cp],cf->d_name,strlen(cf->d_name));cp+=strlen(cf->d_name);(*fn)[cp]='\n';cp+=1;
	}
	(*fn)[fns]='\0';
	(*str)[sns]='\0';

	if (file!=NULL){fclose(file);}
	closedir(dir);

	*strs=sns;
	*fs=fns;
}
