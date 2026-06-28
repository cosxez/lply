#pragma once

struct cnf
{
	char bgi[512];
	char lmd[512];
	char ip[128];
	unsigned short port;
	char thie;
	char wm;
	unsigned char tc[3];
};

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

typedef struct
{
	char *mlist;
	unsigned int mlistrm;
	unsigned int tgmi;
	unsigned int tlmi;
	char *flist;
	unsigned int flists;
	unsigned int tgfi;
	unsigned int mlisti;
	unsigned int mlistio;
	char *is_busy;
	char *ld;
	unsigned char **mbuff;
	unsigned int *sbuff;
	ma_engine *eng;
	ma_decoder *decoder;
	ma_sound *sound;
	unsigned long long int *mcp;
	int *sock;
	struct sockaddr_in *faddr;

	char *ra;
}TSlply_capt;
