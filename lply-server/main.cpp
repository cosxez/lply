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

void udp_client(int *sock,struct sockaddr_in faddr,struct cnf *conf)
{
	struct sockaddr_in ffaddr;
	socklen_t ips=sizeof(ffaddr);
	unsigned short con_mgc=0xac5f;
	unsigned short mgcfe=0xe1dd;
	sendto(*sock,&con_mgc,sizeof(con_mgc),0,(struct sockaddr*)&faddr,sizeof(faddr));

	std::string mlist;
	std::vector<const char*> flist;
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
				flist.push_back((c.path().filename().string()).c_str());
				mlist+=std::string(tcfm,sns)+'\n';
			}
		}
	}
	size_t mlists=mlist.size();
	sendto(*sock,&mlists,sizeof(mlists)-1,0,(struct sockaddr*)&faddr,sizeof(faddr));
	sendto(*sock,mlist.c_str(),mlists-1,0,(struct sockaddr*)&faddr,sizeof(faddr));

	unsigned int mli;
	ssize_t sb=recvfrom(*sock,&mli,sizeof(mli),0,(struct sockaddr*)&ffaddr,&ips);
	std::cout<<"was read\n";
	if (faddr.sin_addr.s_addr==ffaddr.sin_addr.s_addr)
	{
		unsigned int smli=0;
		std::string sfmi;
		for (unsigned int i=0;i<mlists;i++)
		{
			if (mlist[i]=='\n'){smli+=1;if (smli==mli){break;}sfmi="";}
			sfmi+=mlist[i];
		}

		std::cout<<flist[smli-1]<<std::endl;
		std::ifstream file(std::string(conf->dmp)+'/'+flist[smli-1],std::ios::binary);
		if (file.is_open())
		{
			std::cout<<"opened\n";
			file.seekg(0,std::ios::end);
			size_t fs=file.tellg();
			file.seekg(0,std::ios::beg);

			unsigned char bfs[fs];
			file.read(reinterpret_cast<char*>(&bfs),fs);
			file.close();

			sendto(*sock,&fs,sizeof(fs),0,(struct sockaddr*)&faddr,sizeof(faddr));
			size_t cpc=0;
			while (cpc<fs)
			{
				if ((cpc+1024)<fs)
				{
					sendto(*sock,&bfs[cpc],1024,0,(struct sockaddr*)&faddr,sizeof(faddr));
					cpc+=1024;
				}
				else
				{
					sendto(*sock,&bfs[cpc],fs-cpc,0,(struct sockaddr*)&faddr,sizeof(faddr));
					cpc+=fs-cpc;
				}
			}
			unsigned short mgfer=0x3bad;
			sendto(*sock,&mgfer,sizeof(mgfer),0,(struct sockaddr*)&faddr,sizeof(faddr));
		}
	}
	std::cout<<"client disconnected\n";
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

		sendto(*sock,&fs,sizeof(fs),0,(struct sockaddr*)&faddr,sizeof(faddr));
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
			if (sb>2 && *(unsigned short*)mgc==0x6e9d){std::thread(udp_smd,sock,faddr,conf,std::string(&mgc[2],sb-2)).detach();std::cout<<"get UDP \"gmd\" request\n";}
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
