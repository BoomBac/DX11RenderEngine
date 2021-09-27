#include <utility>
#include <fstream>
#include <sstream>
#include <set>

#include <QDebug>


#include "Public/Render/ModelResFactory.h"
#include "Public/Render/Bindable/VertexBuffer.hpp"
#include "Public/Render/VertexLayout.h"
#include "Public/Render/Bindable/IndexBuffer.h"
#include "Public/Render/Bindable/VertexShader.h"
#include "Public/Render/Bindable/PixelShader.h"
#include "Public/Render/Bindable/InputLayout.h"
#include "vector3d.h"
#include "vector2d.h"


using namespace std;


template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::shared_ptr<BindableInterface>;


std::map<int, std::vector<std::shared_ptr<BindableInterface>>> ModelResFactory::res_pool_;
Graphics* ModelResFactory::gfx_;

inline bool ModelResFactory::Exist(int model_key)
{
	return res_pool_.find(model_key) != res_pool_.end();
}

inline bool ModelResFactory::Exist(int model_key, std::map<int, std::vector<std::shared_ptr<BindableInterface>>>::iterator& it)
{
	it = res_pool_.find(model_key);
	return it != res_pool_.end();
}

ModelResFactory::ModelResFactory(Graphics* gfx)
{
	gfx_ = gfx;
}

std::vector<std::shared_ptr<BindableInterface>>& ModelResFactory::GetResource(const int& model_key)
{
	std::map<int, std::vector<std::shared_ptr<BindableInterface>>>::iterator it;
	if (!Exist(model_key, it))
		return std::vector<std::shared_ptr<BindableInterface>>{};
	else
		return it->second;
}
namespace
{
	std::vector<Postion3DTN> g_vertices;
	std::vector<UINT> g_indices;
}

bool ModelResFactory::AddResource(int model_key)
{
	if (Exist(model_key)) return false;
	LoadFile(string{});
	std::vector<std::shared_ptr<BindableInterface>> v;

	int size = 5.f;

	//{
	//	{{0.f + size,0.f + size,0.f + size},{1.f,0.f,0.f}},
	//	{{0.f + size,0.f + size,0.f - size},{1.f,0.f,0.f}},
	//	{{0.f - size,0.f + size,0.f - size},{1.f,0.f,0.f}},
	//	{{0.f - size,0.f + size,0.f + size},{1.f,0.f,0.f}},
	//	{{0.f + size,0.f - size,0.f + size},{0.f,1.f,0.f}},
	//	{{0.f + size,0.f - size,0.f - size},{0.f,1.f,0.f}},
	//	{{0.f - size,0.f - size,0.f - size},{0.f,1.f,0.f}},
	//	{{0.f - size,0.f - size,0.f + size},{0.f,1.f,0.f}}
	//};
	v.push_back(std::make_shared<VertexBuffer<Postion3DTN, Vec>>(g_vertices, *gfx_));

	//{
	//	3,0,1,
	//	3,1,2,//top
	//	1,0,4,
	//	1,4,5,//right
	//	2,1,5,
	//	2,5,6,//front
	//	6,5,4,
	//	6,4,7,//bot
	//	3,2,6,
	//	3,6,7,//left
	//	0,3,7,
	//	7,4,0//back
	//};
	auto vs = std::make_shared<VertexShader>(*gfx_, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/VertexShader.cso");
	v.push_back(std::make_shared<IndexBuffer>(g_indices, *gfx_));
	v.push_back(std::make_shared<PixelShader>(*gfx_, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/PixelShader.cso"));
	VertexLayout vl;
	vl << VertexType::Position3D << VertexType::Texture2D << VertexType::Normal;
	v.push_back(std::make_shared<InputLayout>(*gfx_, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	v.push_back(vs);
	res_pool_.insert(std::pair<int, std::vector<std::shared_ptr<BindableInterface>>>(model_key, v));
}

void ModelResFactory::LoadFile(std::string file_path)
{
	//ifstream ifs("C:\\Users\\BoomBac\\Desktop\\box.obj", ios::in);
	//ifstream ifs("C:\\Users\\BoomBac\\Desktop\\aa.obj", ios::in);
	ifstream ifs("C:\\Users\\BoomBac\\Desktop\\zzz.obj",ios::in);
	if (!ifs.is_open())
	{
		qDebug() << "open failed";
		ifs.close();
		return;
	}
	string buffer;
	vector<CusMath::vector3d> postion;
	vector<CusMath::vector2d> uv;
	vector<CusMath::vector3d> normal;

	vector<Postion3DTN> vb;
	vector<unsigned int> ib;
	float x, y, z;
	while (!ifs.eof())
	{
		getline(ifs, buffer);
		stringstream ss(buffer);
		string type;
		ss >> type;
		if (type == "v")
		{
			ss >> x;
			ss >> y;
			ss >> z;
			postion.push_back(CusMath::vector3d{ x,y,z });
		}
		else if (type == "vt")
		{
			ss >> x;
			ss >> y;
			uv.push_back(CusMath::vector2d{ x,y });
		}
		else if (type == "vn")
		{
			ss >> x;
			ss >> y;
			ss >> z;
			normal.push_back(CusMath::vector3d{ x,y,z });
		}
		else if (type == "f")
		{
			static int row = 0;
			auto s = ss.str();
			int count = -1;
			for (int i = 0; i < s.length(); i++)
			{
				if (s[i] == 'f' || s[i] == '/')
				{
					s[i] = ' ';
					count++;
				}
			}
			stringstream l_ss(s);
			count /= 2;
			static set<float> sett;
			if (count == 4)
			{
				int arr[12];
				for (int i = 0; i < 12; i++)
				{
					l_ss >> arr[i];
					arr[i] -= 1;
				}
				if (sett.find(arr[2]) == sett.end())
				{
					sett.insert(arr[2]);
					g_vertices.push_back(Postion3DTN{ postion[arr[0]], uv[arr[1]], normal[arr[2]] });
				}
				if (sett.find(arr[5]) == sett.end())
				{
					sett.insert(arr[5]);
					g_vertices.push_back(Postion3DTN{ postion[arr[3]], uv[arr[4]], normal[arr[5]] });
				}
				if (sett.find(arr[8]) == sett.end())
				{
					sett.insert(arr[8]);
					g_vertices.push_back(Postion3DTN{ postion[arr[6]], uv[arr[7]], normal[arr[8]] });
				}
				if (sett.find(arr[11]) == sett.end())
				{
					sett.insert(arr[11]);
					g_vertices.push_back(Postion3DTN{ postion[arr[9]], uv[arr[10]], normal[arr[11]] });
				}
				g_indices.push_back(arr[2]);
				g_indices.push_back(arr[5]);
				g_indices.push_back(arr[8]);
				g_indices.push_back(arr[2]);
				g_indices.push_back(arr[8]);
				g_indices.push_back(arr[11]);
			}
			else 
			{
				int arr[9];
				for (int i = 0; i < 9; i++)
				{
					l_ss >> arr[i];
					--arr[i];
				}
				if (sett.find(arr[2]) == sett.end())
				{
					sett.insert(arr[2]);
					g_vertices.push_back(Postion3DTN{ postion[arr[0]], uv[arr[1]], normal[arr[2]] });
				}
				if (sett.find(arr[5]) == sett.end())
				{
					sett.insert(arr[5]);
					g_vertices.push_back(Postion3DTN{ postion[arr[3]], uv[arr[4]], normal[arr[5]] });
				}
				if (sett.find(arr[8]) == sett.end())
				{
					sett.insert(arr[8]);
					g_vertices.push_back(Postion3DTN{ postion[arr[6]], uv[arr[7]], normal[arr[8]] });
				}
				g_indices.push_back(arr[2]);
				g_indices.push_back(arr[5]);
				g_indices.push_back(arr[8]);	
			}
		}
		else
		{
			continue;
		}
	}
	ifs.close();
}

