#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "cfg_pars.h"

int main()
{
	struct cnf conf;
	if (cfg_pars(&conf)!=0){return -1;}
	
	return 0;
}
