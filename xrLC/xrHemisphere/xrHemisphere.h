#pragma once

#ifdef XRHEMISPHERE_EXPORTS
#define XRHS_API __declspec(dllexport)
#else
#define XRHS_API __declspec(dllimport)
#endif

typedef void __stdcall		xrHemisphereIterator(float x, float y, float z, float scale, LPVOID param);

extern "C" 
{
	// Returns TRUE only if everything OK.
	XRHS_API VOID	__cdecl xrHemisphereBuild
		(
			int						quality,
			BOOL					ground,
			float					ground_scale,
			xrHemisphereIterator*	iterator,
			LPVOID					param
		);
};
