// occRasterizer.h: interface for the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
#define AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_
#pragma once

const int	occ_dim0			= 64+2;
const int	occ_dim1			= (occ_dim0-2)/2;
const int	occ_dim2			= occ_dim1/2;
const int	occ_dim3			= occ_dim2/2;

class occTri
{
public:	
	occTri*			adjacent	[3];
	Fvector			raster		[3];		
};

class occRasterizer  
{
private:
	occTri*			bufFrame	[occ_dim0][occ_dim0];
	float			bufDepth0	[occ_dim0][occ_dim0];
	float			bufDepth1	[occ_dim1][occ_dim1];
	float			bufDepth2	[occ_dim2][occ_dim2];
	float			bufDepth3	[occ_dim3][occ_dim3];
public:
	IC int			d2int		(float d)	{ return iFloor(d*float(0x40000000));	}
	IC float		d2float		(int d)		{ return float(d)/float(0x40000000);	}

	void			clear		();
	void			propagade	();
	void			rasterize	(occTri* T);
	
	float*			get_depth	()	{ return &(bufDepth0[0][0]);	}
	occTri**		get_frame	()	{ return &(bufFrame[0][0]);		}

	occRasterizer	();
	~occRasterizer	();
};

extern occRasterizer	Raster;

#endif // !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
