#pragma once

struct cnf
{
	char bgi[512];
	char lmd[512];
	char ip[128];
	unsigned short port;
	char thie;
	char wm;
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
