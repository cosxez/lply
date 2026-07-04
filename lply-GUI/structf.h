#pragma once

struct cnf
{
	char bgi[512];
	char lmd[512];
	char ip[128];
	int win_width;
	int win_height;
	unsigned int drsbuff;
	unsigned short port;
	char thie;
	char wm;
	char spblm;
	unsigned char tc[3];
	unsigned char cc[3];
	char bluc;
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
	char rstate;
};

typedef struct
{
	unsigned char *mba;
	unsigned int mbs;
	
	char *mn;
	unsigned int mns;

	unsigned int idx;
}SRAMm;

typedef struct
{
	SRAMm *rmb;
	unsigned short rmbs;
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
	unsigned int *mds;
	ma_engine *eng;
	ma_decoder *decoder;
	ma_sound *sound;
	unsigned long long int *mcp;
	int *sock;
	struct sockaddr_in *faddr;
	unsigned int *nls;

	char *ra;
}TSlply_capt;

typedef struct
{
	SRAMm *rmb;
	unsigned short rmbs;
	unsigned int tidx;
	unsigned char **mbuff;
	unsigned int *sbuff;
	unsigned int *mds;
	unsigned long long int *mcp;
	ma_engine* eng;
	ma_sound* sound;
	ma_decoder* decoder;
	char *is_busy;
}TSlply_ptfr;
