#ifndef EToolsH
#define EToolsH

#ifdef CREATEDX_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

namespace ETOOLS{
	extern "C" ETOOLS_API bool mvc_TestRayTri	(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull);
	extern "C" ETOOLS_API bool mvc_TestRayTri2	(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull);
};

#endif // EToolsH