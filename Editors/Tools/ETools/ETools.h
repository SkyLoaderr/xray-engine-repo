#ifndef EToolsH
#define EToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

extern "C" {
	// fast functions
	namespace ETOOLS{
		ETOOLS_API bool TestRayTriA		(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool TestRayTriB		(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool TestRayTri2		(const Fvector& C, const Fvector& D, Fvector* p, float& range);
	};
};

#endif // EToolsH