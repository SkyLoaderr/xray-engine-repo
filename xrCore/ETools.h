#ifndef EToolsH
#define EToolsH

#ifdef CREATEDX_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

#include "D3DX_Wrapper.h"

namespace ETOOLS{
	extern "C" 
	{
		ETOOLS_API bool TestRayTri	(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool TestRayTri2	(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull);
	};
};

#endif // EToolsH