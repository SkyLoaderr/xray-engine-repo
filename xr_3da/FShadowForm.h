// FShadowForm.h: interface for the FShadowForm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSHADOWFORM_H__DEB2957E_816E_4DA4_9D5E_D959286000C6__INCLUDED_)
#define AFX_FSHADOWFORM_H__DEB2957E_816E_4DA4_9D5E_D959286000C6__INCLUDED_
#pragma once

#include "FBasicVisual.h"

// These are the shadow volumes
struct SHADOW_V
{
	WORD*   pwShadVolIndices;      // tri indices into vertex buffer VB for DrawPrim
	WORD*   pwShadVolSideIndices;  // ptrs into main index array pwShadVolIndices for Side tris of shadow volume
	WORD*   pwShadVolCapIndices;   // ptrs into main index array pwShadVolIndices for cap tris of shadow volume
	DWORD   dwNumVertices;
	DWORD   dwNumSideIndices;
	DWORD   dwNumCapIndices;
};
#pragma pack(push)
#pragma pack(1)
struct COLORVERTEX {
	Fvector p;
	DWORD	c;
};
#pragma pack(pop)

class FShadowForm : public FBasicVisual
{
	// Data
	LPDIRECT3DVERTEXBUFFER7	pVB;
	vector<WORD>			Indices;
	DWORD					dwNumVertices;

	BOOL					g_bDrawShadowVolCaps;

	// Resulting shadow volume
	LPDIRECT3DVERTEXBUFFER7	pProjected;
	SHADOW_V				S;
	WORD*					pwCHI;

	// Private methods
public:
	void					RenderShadow		(LPDIRECT3DVERTEXBUFFER7 pv);
	void					MakeShadowVolume	(_vector &L, _matrix &mWorld );

	virtual FBasicVisual*	CreateInstance(void);
	virtual	void			Load(CStream *data, DWORD dwFlags);
	virtual void			Render(float LOD);

	FShadowForm();
	virtual ~FShadowForm();

};

#endif // !defined(AFX_FSHADOWFORM_H__DEB2957E_816E_4DA4_9D5E_D959286000C6__INCLUDED_)
