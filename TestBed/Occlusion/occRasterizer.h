// occRasterizer.h: interface for the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
#define AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_
#pragma once

const int	occ_dim0			= 64;
const int	occ_dim1			= 32;
const int	occ_dim2			= 16;
const int	occ_dim3			= 8;

struct Fvector
{
	float x,y,z;
};

class occTri
{
public:	
	occTri*			adjacent	[3];
	Fvector			raster		[3];		
};

class occRasterizer  
{
	enum			Sections	{BOTTOM, TOP};
private:
	occTri*			bufFrame	[occ_dim0][occ_dim0];
	float			bufDepth0	[occ_dim0][occ_dim0];
	float			bufDepth1	[occ_dim1][occ_dim1];
	float			bufDepth2	[occ_dim2][occ_dim2];
	float			bufDepth3	[occ_dim3][occ_dim3];
private:
	void			i_order		(float* a, float* b, float* c);
	void			i_scan		(int curY, float startX, float endX, float startZ, float endZ, occTri* T);
	void			i_section	(float* a, float* b, float* c, occTri* T, int id);
public:
	void			clear		();
	void			propagade	();
	void			rasterize	(occTri* T);
	
	float*			dbg_depth	()	{ return &(bufDepth0[0][0]);	}
	occTri**		dbg_frame	()	{ return &(bufFrame[0][0]);		}

	occRasterizer	();
	~occRasterizer	();
};

#endif // !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
