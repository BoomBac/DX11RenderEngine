#include <fstream>
#include <sstream>
#include <set>
#include <stack>

#include <QDebug>

#include "Public/Render/ResManage/MeshFactory.h"
#include <Public/Render/ModelResFactory.h>


using namespace std;

namespace
{
	
}

MeshFactory& MeshFactory::getInstance()
{
	static MeshFactory instance;
	return instance;
}

bool MeshFactory::AddMesh(std::string file_path)
{
	if (mesh_loaded_.find(file_path) != mesh_loaded_.end()) return false;
	vector<Postion3DTN> vertics;
	vector<UINT> indices;
	LoadMesh(file_path, vertics, indices);
	const char* file_name;
	bool can_push = false;
	for (int i = file_path.length() - 1; i > 0; i--)
	{
		if (file_path[i] == '.')
		{
			can_push = true;
			continue;
		}
		if (can_push)
		{
			if (file_path[i] == '/')
			{
				file_name = &file_path[i + 1];
				break;
			}
		}
	}
	vertices_pool_.insert(pair<string, vector<Postion3DTN>>(file_name, move(vertics)));
	indices_pool_.insert(pair<string, vector<UINT>>(file_name, move(indices)));
	ModelResFactory::GetInstance().AddResource(file_name);
	mesh_loaded_.insert(file_path);
	return true;
}

bool MeshFactory::GetMesh(std::string file_path, std::vector<Postion3DTN>** pv, std::vector<UINT>** pi)
{
	auto v_it = vertices_pool_.find(file_path);
	if (v_it != vertices_pool_.end())
		*pv = &(v_it->second);
	else
		return false;
	auto i_it = indices_pool_.find(file_path);
	*pi = &(i_it->second);
	return true;
}

void MeshFactory::LoadMesh(std::string file_path, std::vector<Postion3DTN>& vertics, std::vector<UINT>& indices)
{
	EMeshType g_mesh_type = EMeshType::kNormalOnly;
	ifstream ifs(file_path, ios::in);
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
	float x, y, z;
	set<float> sett;
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
			g_mesh_type = EMeshType::kUVAndNormal;
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
			if (g_mesh_type == EMeshType::kUVAndNormal)
			{
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
				count = count / 2 * 3;
				int* arr = new int[count];
				for (int i = 0; i < count; i++)
				{
					l_ss >> arr[i];
					arr[i] -= 1;
				}
				for (int i = 0; i < count - 2; i += 3)
				{
					if (sett.find(arr[i + 2]) == sett.end())
					{
						sett.insert(arr[i + 2]);
						vertics.push_back(Postion3DTN{ postion[arr[i]], uv[arr[i + 1]], normal[arr[i + 2]] });
					}
					if (i >= 6)
					{
						indices.push_back(arr[2]);
						indices.push_back(arr[i - 1]);
						indices.push_back(arr[i + 2]);
					}
				}
				delete[] arr;
			}
			if (g_mesh_type == EMeshType::kNormalOnly)
			{
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
				static set<float> sett;
				int* arr = new int[count];
				for (int i = 0; i < count; i++)
				{
					l_ss >> arr[i];
					arr[i] -= 1;
				}
				for (int i = 0; i < count - 1; i += 2)
				{
					if (sett.find(arr[i + 1]) == sett.end())
					{
						sett.insert(arr[i + 1]);
						vertics.push_back(Postion3DTN{ postion[arr[i]], CusMath::vector2d{0.f,0.f}, normal[arr[i + 1]] });
					}
					if (i >= 4)
					{
						indices.push_back(arr[1]);
						indices.push_back(arr[i - 1]);
						indices.push_back(arr[i + 1]);
					}
				}
				delete[] arr;
			}
		}
	}
	ifs.close();
}