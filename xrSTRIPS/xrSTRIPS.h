#ifndef XRSTRIPS
#define XRSTRIPS

#ifdef XRSTRIPS_EXPORTS
#define SP_API __declspec(dllexport)
#else
#define SP_API __declspec(dllimport)
#endif

#ifndef IC
#define IC __forceinline
#endif

extern "C" {

// Strippifier and vertex cache reordering
SP_API void __cdecl xrStripify (
	std::vector<WORD> &indices, 
	std::vector<WORD> &perturb, 
	int iCacheSize, int iMinStripLength
	);

// Vertex cache simulator - return number of vertex TnL'ed
SP_API int __cdecl xrSimulate (
	std::vector<WORD> &indices, 
	int iCacheSize
	);
}

#endif