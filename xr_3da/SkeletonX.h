// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKELETONX_H__24A6DC9C_8CE0_4A45_AF43_DBFF8EA3638A__INCLUDED_)
#define AFX_SKELETONX_H__24A6DC9C_8CE0_4A45_AF43_DBFF8EA3638A__INCLUDED_
#pragma once

#include "FVisual.h"
#include "FProgressiveFixedVisual.h"

// refs
class ENGINE_API CKinematics;

#pragma pack(push,4)
struct vertBoned1W	// (3+3+2+1)*4 = 9*4 = 36 bytes
{
	Fvector	P;
	Fvector	N;
	float	u,v;
	u32	matrix;
};
struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	WORD	matrix0;
	WORD	matrix1;
	Fvector	P0;
	Fvector	N0;
	Fvector	P1;
	Fvector	N1;
	float	w;
	float	u,v;
};
struct vertRender
{
	Fvector	P;
	Fvector	N;
	float	u,v;
};
#pragma pack(pop)

class ENGINE_API CSkeletonX
{
protected:
	enum { vertRenderFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 };

	CKinematics*			Parent;			// setted up by parent
	vertBoned1W*			Vertices1W;		// shared
	vertBoned2W*			Vertices2W;		// shared
	u32					cache_DiscardID;
	u32					cache_vCount;
	u32					cache_vOffset;
	
	void					_Copy			(CSkeletonX *V);
	void					_Render			(CVS* hVS, u32 vCount, u32 pCount, IDirect3DIndexBuffer8* IB);
	void					_Release		();
	void					_Load			(const char* N, CStream *data, u32& dwVertCount);
public:
	virtual void			SetParent		(CKinematics* K) { Parent = K; }

	CSkeletonX()
	{
		Parent		= 0;
		Vertices1W	= 0;
		Vertices2W	= 0;
	}
};

class ENGINE_API CSkeletonX_ST : public Fvisual, public CSkeletonX
{
private:
	typedef Fvisual	inherited;
public:
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, CStream *data, u32 dwFlags);
	virtual void			Copy			(CVisual *pFrom);
	virtual void			Release			();
};

class ENGINE_API CSkeletonX_PM : public FProgressiveFixedVisual, public CSkeletonX
{
private:
	typedef FProgressiveFixedVisual	inherited;
	WORD*					indices;
public:
	CSkeletonX_PM()	: indices(0) {};
	
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, CStream *data, u32 dwFlags);
	virtual void			Copy			(CVisual *pFrom);
	virtual void			Release			();
};

#endif // !defined(AFX_SKELETONX_H__24A6DC9C_8CE0_4A45_AF43_DBFF8EA3638A__INCLUDED_)
