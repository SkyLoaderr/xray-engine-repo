// occRasterizer.h: interface for the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
#define AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_
#pragma once

const int	occ_dim_0			= 64;
const int	occ_dim_1			= occ_dim_0/2;
const int	occ_dim_2			= occ_dim_1/2;
const int	occ_dim_3			= occ_dim_2/2;
const int	occ_dim				= occ_dim_0+2;	// 1 pixel border around frame

class occTri
{
public:	
	occTri*			adjacent	[3];
	Fvector			raster		[3];		
};

class occRasterizer  
{
private:
	occTri*			bufFrame	[occ_dim][occ_dim];
	float			bufDepth	[occ_dim][occ_dim];

	float			bufDepth_0	[occ_dim_0][occ_dim_0];
	float			bufDepth_1	[occ_dim_1][occ_dim_1];
	float			bufDepth_2	[occ_dim_2][occ_dim_2];
	float			bufDepth_3	[occ_dim_3][occ_dim_3];
public:
	IC int			d2int		(float d)	{ return iFloor(d*float(0x40000000));	}
	IC float		d2float		(int d)		{ return float(d)/float(0x40000000);	}

	void			clear		();
	void			propagade	();
	void			rasterize	(occTri* T);
	
	occTri**		get_frame	()			{ return &(bufFrame[0][0]);	}
	float*			get_depth	()			{ return &(bufDepth[0][0]);	}
	float*			get_depth	(int level)	
	{
		switch (level)
		{
		case 0:		return &(bufDepth_0[0][0]);	
		case 1:		return &(bufDepth_1[0][0]);	
		case 2:		return &(bufDepth_2[0][0]);	
		case 3:		return &(bufDepth_3[0][0]);	
		default:	return NULL;
		}
	}
	
	occRasterizer	();
	~occRasterizer	();
};

extern occRasterizer	Raster;

#endif // !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
