#pragma once

#ifdef XRDXTC_EXPORTS
#define DXTC_API __declspec(dllexport)
#else
#define DXTC_API __declspec(dllimport)
#endif

enum eDXTC
{
	eDXT1=1,
	eDXT3=3,
	eDXT5=5
};

extern "C" {
	// Returns TRUE only if everything OK.
	DXTC_API BOOL	__cdecl xrDXTC_Compress(
		const char *dest_file,
		DWORD		dest_format,
		BOOL		dest_mipmaps,
		DWORD*		src_data,
		DWORD		dwWidth,
		DWORD		dwHeight,
		DWORD		dxt1_alpharef = 0xff
		);
};
