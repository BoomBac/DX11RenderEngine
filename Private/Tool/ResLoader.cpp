#include <fstream>
#include <sstream>

#include "Public/Tool/ResLoader.h"


using namespace::std;

bool ResLoader::LoadHDRHeader(const char* path, HDRHeader& header)
{
	ifstream ifs;
	ifs.open(path, ios::in);
	if (!ifs.is_open()) return -1;
	char buf[128];
	ifs.getline(buf, 16);
	if (memcmp(buf, "#?RADIANCE", 10))
	{
		ifs.close();
		return false;
	}
	memcpy(header.programtype, buf, 16);
	while (true)
	{
		ifs.getline(buf, 64);
		if (!memcmp(buf, "EXPOSURE", 8))
		{
			//size_t start = 0;
			//for (auto c : buf)
			//{
			//	++start;
			//	if (c == '=') break;
			//}
			string str(&buf[9]);
			stringstream ss(str);
			ss >> header.exposure;
			ss.clear();
			ss.str("");
		}
		else if (!memcmp(buf, "GAMMA", 5))
		{
			string str(&buf[6]);
			stringstream ss(str);
			ss >> header.gamma;
			ss.clear();
			ss.str("");
		}
		else if (buf[0] == '\0') { break; }
	}
	//ifs.getline(buf, 64);
	string str;
	getline(ifs,str);
	stringstream ss;
	ss << str;
	ss >> str;
	ss >> header.height;
	ss >> str;
	ss >> header.width;
	ifs.close();
	return true;
}

bool ResLoader::LoadHDRFile(const char* path, int width, int height, float* pixel)
{
	//ifstream ifs(path, ios::binary);
	////| ios::binary
	//if (!ifs.is_open()) return false;
	//char buf[64];
	//while (true)
	//{
	//	ifs.getline(buf, 64);
	//	if (buf[0] == '\0') break;
	//}
	//ifs.getline(buf, 64);
	//int pxiel_count = width * height + 1;
	//int pixel_num = 0;
	//while (--pxiel_count)
	//{
	//	ifs.read(buf, 4);
	//	if (buf[3])
	//	{
	//		float e = pow(2.f, (static_cast<float>(static_cast<unsigned char>(buf[3])) - 136));
	//		pixel[pixel_num] = static_cast<float>(static_cast<unsigned char>(buf[0]))* e;
	//		pixel[pixel_num + 1] = static_cast<float>(static_cast<unsigned char>(buf[1]))* e;
	//		pixel[pixel_num + 2] = static_cast<float>(static_cast<unsigned char>(buf[2]))* e;
	//		pixel[pixel_num + 3] = 1.f;
	//	}
	//	else
	//	{
	//		pixel[pixel_num] = pixel[pixel_num + 1] = pixel[pixel_num + 2] = 0.f;
	//		pixel[pixel_num + 3] = 1.f;
	//	}
	//	pixel_num += 4;
	//}
	//ifs.close();
	return true;
}

void ResLoader::WriteToFile(const char* path, const float* pixel, const int& pixel_num)
{
	ofstream ofs(path,ios::in);
	stringstream ss;
	for (int i = 0; i < pixel_num * 4; i+=4)
	{
		ss << "r:" << pixel[i] << " g:" << pixel[i + 1] << " b:" << pixel[i + 2] << " a:" << pixel[i + 3] << '\n';
		ofs << ss.str();
		ss.clear();
		ss.str("");
	}
	ofs.close();
}
