
#ifndef DX11ENGINE_TOOL_RESLOADER_H
#define DX11ENGINE_TOOL_RESLOADER_H



struct HDRHeader
{
	char programtype[16];
	float gamma;
	float exposure;
	int width;
	int height;
};

class ResLoader {
public:
	static bool LoadHDRHeader(const char* path, HDRHeader& header);
	static bool LoadHDRFile(const char* path, int width, int height, float* pixel);
	static void WriteToFile(const char* path, const float* pixel,const int& pixel_num);
};


#endif //DX11ENGINE_TOOL_RESLOADER_H


