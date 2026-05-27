#include <fstream>

int main(int args,char *argv[])
{
	if (args==3)
	{
		std::ifstream fr(argv[1],std::ios::binary);
		fr.seekg(0,std::ios::end);
		unsigned int fs=fr.tellg();
		fr.seekg(0,std::ios::beg);

		unsigned char buff[fs];
		fr.read(reinterpret_cast<char*>(&buff),fs);
		fr.close();
		
		std::ofstream fw(argv[2],std::ios::binary | std::ios::app);
		fw.seekp(0,std::ios::end);
		fw.write(reinterpret_cast<const char*>(&buff),fs);
		fw.write(reinterpret_cast<const char*>(&fs),sizeof(fs));
		fw.close();
	}
}
