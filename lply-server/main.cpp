#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>
#include <filesystem>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct cnf
{
	char dmp[256];
	unsigned short dmps=0;
	char with_s;
};

char cfgp(struct cnf *conf)
{
	std::ifstream file("config");
	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string cl;
			std::string arg="";
			std::string pr="";

			getline(file,cl);
			int cpos=0;
			for (int i=0;cl[i]!='=';i++)
			{
				if (i>=cl.size()){break;}
				arg+=cl[i];
				cpos=i;
			}
			if (cpos+2<cl.size())
			{
				cpos+=2;
				for (cpos;cpos<cl.size();cpos++)
				{
					pr+=cl[cpos];
				}
			}
			if (arg=="dir-path" && pr.size()<256)
			{
				unsigned char i=0;
				for (i;i<pr.size();i++)
				{
					conf->dmp[i]=pr[i];
				}
				conf->dmp[i]='\0';conf->dmps=i;
			}
			if (arg=="with-retry")
			{
				conf->with_s=pr[0];
			}
		}
	}
	else{return 1;}
	
	return 0;
}

void sclose(int *sock)
{
	if (*sock<0){return;}
	else{close(*sock);*sock=-1;}
}

void udp_smlist(int *sock,struct sockaddr_in faddr,struct cnf *conf)
{
	std::string mlist;
	std::string flist;
	for (auto &c:std::filesystem::directory_iterator(conf->dmp))
	{
		if (!std::filesystem::is_directory(c))
		{
			std::ifstream file(std::string(conf->dmp)+'/'+(c.path().filename().string()),std::ios::binary);
			if (file.is_open())
			{
				unsigned short sns;file.read(reinterpret_cast<char*>(&sns),2);
				char tcfm[sns];
				file.read(reinterpret_cast<char*>(&tcfm),sns);
				file.close();
				flist+=(c.path().filename().string())+'\n';
				mlist+=std::string(tcfm,sns)+'\n';
			}
		}
	}
	size_t mlists=mlist.size();
	sendto(*sock,&mlists,sizeof(mlists),0,(struct sockaddr*)&faddr,sizeof(faddr));
	sendto(*sock,mlist.c_str(),mlists,0,(struct sockaddr*)&faddr,sizeof(faddr));
	
	size_t flists=flist.size();
	sendto(*sock,&flists,sizeof(flists),0,(struct sockaddr*)&faddr,sizeof(faddr));
	sendto(*sock,flist.c_str(),flists,0,(struct sockaddr*)&faddr,sizeof(faddr));
	std::cout<<"\"mlist\" sended\n";
}

void udp_smd(int *sock,struct sockaddr_in faddr,struct cnf *conf,std::string mfn)
{
	std::ifstream file(std::string(conf->dmp)+'/'+mfn,std::ios::binary);
	if (file.is_open())
	{
		file.seekg(0,std::ios::end);
		size_t fs=file.tellg();
		file.seekg(0,std::ios::beg);

		std::vector<unsigned char> bfs(fs);
		file.read(reinterpret_cast<char*>(bfs.data()),fs);
		file.close();

		size_t cpc=0;
		while (cpc<fs)
		{
			if ((cpc+1024)<fs)
			{
				sendto(*sock,bfs.data()+cpc,1024,0,(struct sockaddr*)&faddr,sizeof(faddr));
				cpc+=1024;
			}
			else
			{
				sendto(*sock,bfs.data()+cpc,fs-cpc,0,(struct sockaddr*)&faddr,sizeof(faddr));
				cpc+=fs-cpc;
			}
		}
		unsigned short mgfer=0xe3dd;
		sendto(*sock,&mgfer,sizeof(mgfer),0,(struct sockaddr*)&faddr,sizeof(faddr));
		std::cout<<"\"md\" sended\n";
	}
}

void udp_smfs(int *sock,struct sockaddr_in faddr,struct cnf *conf,std::string mfn)
{
	std::ifstream file(std::string(conf->dmp)+'/'+mfn,std::ios::binary);
	if (file.is_open())
	{
		file.seekg(0,std::ios::end);
		size_t fs=file.tellg();
		file.seekg(0,std::ios::beg);
		file.close();
		char tbuff[10];*(unsigned short*)tbuff=0xa5f1;memcpy(&tbuff[2],&fs,8);
		sendto(*sock,&tbuff,10,0,(struct sockaddr*)&faddr,sizeof(faddr));
		std::cout<<"\"gmfs\" sended\n";
	}
}

void udp_smdp(int *sock,struct sockaddr_in faddr,struct cnf *conf,std::string mfn,unsigned int sp,unsigned short sfr)
{
	std::ifstream file(std::string(conf->dmp)+'/'+mfn,std::ios::binary);
	if (file.is_open())
	{
		file.seekg(sp,std::ios::beg);
		char *bfs=(char*)malloc(sfr);
		file.read(reinterpret_cast<char*>(bfs),sfr);
		file.close();
		size_t crp=0;
		while (crp<sfr)
		{
			unsigned char rbfs[1030];
			*(unsigned int*)rbfs=crp+sp;
			if ((crp+1024)<sfr){*(unsigned short*)(rbfs+4)=1024;memcpy(&rbfs[6],&bfs[crp],1024);sendto(*sock,rbfs,1030,0,(struct sockaddr*)&faddr,sizeof(faddr));crp+=1024;}
			else{*(unsigned short*)(rbfs+4)=sfr-crp;memcpy(&rbfs[6],&bfs[crp],sfr-crp);sendto(*sock,rbfs,6+sfr-crp,0,(struct sockaddr*)&faddr,sizeof(faddr));crp+=sfr-crp;}
		}
		free(bfs);
		unsigned short mgfer=0xe3dd;sendto(*sock,&mgfer,2,0,(struct sockaddr*)&faddr,sizeof(faddr));
		std::cout<<"\"md\" sended\n";
	}
}

void udp_lsconn(int *sock,struct cnf *conf)
{
	struct sockaddr_in faddr;
	socklen_t ips=sizeof(faddr);
	while (1)
	{
		try
		{
			char mgc[1500];
			int sb=recvfrom(*sock,&mgc,sizeof(mgc),0,(struct sockaddr*)&faddr,&ips);
			if (sb==2 && *(unsigned short*)mgc==0xa1b3){std::thread(udp_smlist,sock,faddr,conf).detach();std::cout<<"get UDP \"mlist\" request\n";}
			if (sb>2 && *(unsigned short*)mgc==0x6e9d){std::thread(udp_smdp,sock,faddr,conf,std::string(&mgc[2],sb-8),*(unsigned int*)(mgc+(sb-6)),*(unsigned short*)(mgc+(sb-2))).detach();std::cout<<"get UDP \"gmd\" request\n";}
			if (sb>2 && *(unsigned short*)mgc==0x65f1){std::thread(udp_smfs,sock,faddr,conf,std::string(&mgc[2],sb-2)).detach();std::cout<<"get UDP \"gmfs\" request\n";}
			if (sb>2 && *(unsigned short*)mgc==0x6ebd){std::thread(udp_smd,sock,faddr,conf,std::string(&mgc[2],sb-2)).detach();std::cout<<"get UDP \"gmd\" request\n";}
		}
		catch(std::exception &e){std::cout<<"Error: "<<e.what()<<std::endl;}
	}
	
}

int main()
{
	struct cnf conf;
	if (cfgp(&conf)!=0){std::cout<<"-1\n";return -1;}
	if (conf.dmps<1){std::cout<<"-2\n";return -2;}

	int sock=socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(3580);
	addr.sin_addr.s_addr=INADDR_ANY;
	bind(sock,(struct sockaddr*)&addr,sizeof(addr));

	udp_lsconn(&sock,&conf);

	sclose(&sock);
	return 0;
}
