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
	DWORD	matrix;
};
struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	WORD	Matrix1;
	WORD	Matrix2;
	Fvector	P1;
	Fvector	N1;
	Fvector	P2;
	Fvector	N2;
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
	CVertexStream*			Stream;
	vertBoned1W*			Vertices;		// shared

	void					_Copy			(CSkeletonX *V);
	void					_Render			(DWORD vCount, DWORD pCount, IDirect3DIndexBuffer8* IB);
	void					_Release		();
	void					_Load			(const char* N, CStream *data, DWORD& dwVertCount);
public:
	virtual void			SetParent		(CKinematics* K) { Parent = K; }

	CSkeletonX()
	{
		Parent		= 0;
		Stream		= 0;
		Vertices	= 0;
	}
};

class ENGINE_API CSkeletonX_ST : public Fvisual, public CSkeletonX
{
private:
	typedef Fvisual	inherited;
public:
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, CStream *data, DWORD dwFlags);
	virtual void			Copy			(FBasicVisual *pFrom);
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
	virtual void			Load			(const char* N, CStream *data, DWORD dwFlags);
	virtual void			Copy			(FBasicVisual *pFrom);
	virtual void			Release			();
};

#endif // !defined(AFX_SKELETONX_H__24A6DC9C_8CE0_4A45_AF43_DBFF8EA3638A__INCLUDED_)
