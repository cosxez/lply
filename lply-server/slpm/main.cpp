#include <iostream>
#include <vector>
#include <fstream>

int main()
{
	std::cout<<"Enter song name: ";
	std::string sn;
	getline(std::cin,sn);
	
	/*std::cout<<"Enter path to image(WHLD only): ";
	std::string ip;
	getline(std::cin,ip);*/

	std::cout<<"Enter path to song(MP3 only): ";
	std::string sp;
	getline(std::cin,sp);

	std::cout<<"Enter name output file: ";
	std::string on;
	getline(std::cin,on);

	/*std::ifstream imgf(ip,std::ios::binary);
	std::vector<unsigned char> imgd;
	if (imgf.is_open())
	{
		imgf.seekg(0,std::ios::end);
		size_t fs=imgf.tellg();
		imgf.seekg(0,std::ios::beg);
		
		imgd.resize(fs);
		imgf.read(reinterpret_cast<char*>(imgd.data()),imgd.size());
		imgf.close();
	}
	else{std::cout<<"Image file dont exist\n";return -1;}*/

	std::ifstream sf(sp,std::ios::binary);
	std::vector<unsigned char> sd;
	if (sf.is_open())
	{
		sf.seekg(0,std::ios::end);
		size_t fs=sf.tellg();
		sf.seekg(0,std::ios::beg);

		sd.resize(fs);
		sf.read(reinterpret_cast<char*>(sd.data()),sd.size());
		sf.close();
	}
	else{std::cout<<"Song file dont exist\n";return -1;}

	std::ofstream file(on,std::ios::binary);
	unsigned short sns=sn.size();
	std::cout<<sns<<std::endl;
	unsigned int mgc=0xe0f1a4b3;
	file.write(reinterpret_cast<const char*>(&mgc),4);
	file.write(reinterpret_cast<const char*>(&sns),2);file.write(reinterpret_cast<const char*>(sn.data()),sns);
	/*unsigned int imgds=imgd.size();
	std::cout<<imgds<<std::endl;
	file.write(reinterpret_cast<const char*>(&imgds),4);file.write(reinterpret_cast<const char*>(imgd.data()),imgds);*/
	std::cout<<sd.size()<<std::endl;
	file.write(reinterpret_cast<const char*>(sd.data()),sd.size());
	file.close();
	return 0;
}
