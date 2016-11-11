/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#include "utils.h"

#include <windows.h>

extern "C" IMAGE_DOS_HEADER __ImageBase; // MSVC specific, with other compilers use HMODULE from DllMain

std::string cachedModulePath;

std::string GetCurrentModulePath()
{
	if (cachedModulePath.empty())
	{
		// get module path
		char modPath[MAX_PATH];
		memset(modPath, 0, sizeof(modPath));
		GetModuleFileNameA((HMODULE)&__ImageBase, modPath, sizeof(modPath));
		for (size_t i = strlen(modPath); i > 0; i--)
		{
			if (modPath[i - 1] == '\\')
			{
				modPath[i] = 0;
				break;
			}
		}
		cachedModulePath = modPath;
	}
	return cachedModulePath;
}

Vector3 MathUtils::rotationToDirection(Vector3 rotation)
{
	//big thanks to camxxcore's C# code https://github.com/CamxxCore/ScriptCamTool/blob/master/GTAV_ScriptCamTool/Utils.cs
	float retZ = rotation.z * 0.01745329f;
	float retX = rotation.x * 0.01745329f;
	float absX = abs(cos(retX));
	Vector3 retVector = { 0.0,0.0,0.0 };
	retVector.x = (float)-(sin(retZ) * absX);
	retVector.y = (float)cos(retZ) * absX;
	retVector.z = (float)sin(retX);
	return retVector;
}

Vector3 MathUtils::crossProduct(Vector3 a, Vector3 b)
{
	//http://onlinemschool.com/math/assistance/vector/multiply1/
	Vector3 retVector = { 0.0,0.0,0.0 };
	retVector.x = a.y*b.z - a.z*b.y;
	retVector.y = a.z*b.x - a.x*b.z;
	retVector.z = a.x*b.y - a.y*b.x;
	return retVector;
}
