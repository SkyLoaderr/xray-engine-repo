// xrSpherical.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "BoundingSphere.h"
#include "Point.h"
#include "xrSpherical.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
extern "C" {
	XRS_API void __cdecl xrSphere_Minimal(void* data, int count, XRS_Sphere& S)
	{
		using namespace MiniBall;
		BoundingSphere BS = Sphere::miniBall((Point*)data, count);
		S.x = BS.center.x;
		S.y = BS.center.y;
		S.z = BS.center.z;
		S.r = BS.radius;
	}
};
