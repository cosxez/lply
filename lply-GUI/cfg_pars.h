#pragma once

struct cnf
{
	char with_s;
};

short cfg_pars(struct cnf *conf,std::string *ip_addr, unsigned short *port)
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
			if (arg=="ip")
			{
				*ip_addr=pr;
			}
			if (arg=="pt")		
			{
				*port=std::stoi(pr);
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
