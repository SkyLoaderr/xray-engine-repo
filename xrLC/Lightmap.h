// Lightmap.h: interface for the CLightmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTMAP_H__889100E6_CF29_47EA_ABFD_41AE28DAC6B1__INCLUDED_)
#define AFX_LIGHTMAP_H__889100E6_CF29_47EA_ABFD_41AE28DAC6B1__INCLUDED_
#pragma once

// refs
class CDeflector;
struct b_light;

// def
class CLightmap  
{
public:
	b_texture	lm;
public:
	CLightmap					();
	~CLightmap					();

	void	Capture				(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotate, b_light* layer);
	void	Save				();
};

#endif // !defined(AFX_LIGHTMAP_H__889100E6_CF29_47EA_ABFD_41AE28DAC6B1__INCLUDED_)
