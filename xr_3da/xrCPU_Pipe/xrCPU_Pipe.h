#ifndef xrCPU_PipeH
#define xrCPU_PipeH

#pragma once

// Forward references
struct	ENGINE_API vertRender;
struct	ENGINE_API vertBoned1W;
struct	ENGINE_API vertBoned2W;
class	ENGINE_API CBoneInstance;
struct	ENGINE_API CKey;
struct	ENGINE_API CKeyQ;

#ifdef _EDITOR
#define MATRIX		Fmatrix;
#else
template<class T>	struct _matrix;
#define MATRIX		_matrix<float>
#endif

// Skinning processor specific functions
// NOTE: Destination memory is uncacheble write-combining (AGP), so avoid non-linear writes
// D: AGP,			32b aligned
// S: SysMem		non-aligned
// Bones: SysMem	64b aligned
typedef void	__stdcall	xrSkin1W		(vertRender* D, vertBoned1W* S, DWORD vCount, CBoneInstance* Bones);
typedef void	__stdcall	xrSkin2W		(vertRender* D, vertBoned2W* S, DWORD vCount, CBoneInstance* Bones);

// Spherical-linear interpolation of quaternion
// NOTE: Quaternions may be non-aligned in memory
typedef void	__stdcall	xrBoneLerp		(CKey* D, CKeyQ* K1, CKeyQ* K2, float delta);

// Matrix multiplication
typedef void	__stdcall	xrM44_Mul		(MATRIX* D, MATRIX* M1, MATRIX* M2);

// Transfer of geometry into DynamicVertexBuffer & DynamicIndexBuffer with optional xform and index offset
// NOTE: vCount and iCount usually small numbers (for example 20/40)
// vDest: AGP,		 non aligned
// vSrc:  SysMem,	 32b aligned
// iDest: SysMem/AGP,non aligned
// iSrc:  SysMem,    32b aligned
// xform: SysMem,    non aligned, may be NULL
typedef void	__stdcall	xrTransfer		(LPVOID vDest, LPVOID vSrc, DWORD vCount, DWORD vStride,
											 LPWORD iDest, LPWORD iSrc, DWORD iCount, DWORD iOffset,
											 MATRIX* xform);

// Memory routines
typedef void	__stdcall	xrMemFill_32b	(LPVOID ptr,  DWORD count, DWORD value);
typedef void	__stdcall	xrMemFill_8b	(LPVOID ptr,  DWORD count, DWORD value);
typedef void	__stdcall	xrMemCopy_8b	(LPVOID dest, const void* src,  DWORD count);

#pragma pack(push,8)
struct xrDispatchTable
{
	xrSkin1W*			skin1W;
	xrSkin2W*			skin2W;
	xrBoneLerp*			blerp;
	xrM44_Mul*			m44_mul;
	xrTransfer*			transfer;
	xrMemCopy_8b*		memCopy;
	xrMemFill_8b*		memFill;
	xrMemFill_32b*		memFill32;
};
#pragma pack(pop)

// Binder
// NOTE: Engine calls function named "_xrBindPSGP"
typedef void	__cdecl	xrBinder	(xrDispatchTable* T);

#undef MATRIX
#endif
