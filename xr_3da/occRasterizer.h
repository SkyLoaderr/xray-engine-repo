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
const int	occ_dim				= occ_dim_0+4;	// 2 pixel border around frame

class ENGINE_API	occTri
{
public:	
	occTri*			adjacent	[3];
	Fvector			raster		[3];		
	Fplane			plane;
	float			area;
	u32			flags;
	u32			skip;
};

class ENGINE_API	occRasterizer  
{
private:
	occTri*			bufFrame	[occ_dim][occ_dim];
	float			bufDepth	[occ_dim][occ_dim];

	int				bufDepth_0	[occ_dim_0][occ_dim_0];
	int				bufDepth_1	[occ_dim_1][occ_dim_1];
	int				bufDepth_2	[occ_dim_2][occ_dim_2];
	int				bufDepth_3	[occ_dim_3][occ_dim_3];
public:
	IC int			d2int		(float d)	{ return iFloor	(d*float(0x40000000));	}
	IC int			d2int_up	(float d)	{ return iCeil	(d*float(0x40000000));	}
	IC float		d2float		(int d)		{ return float(d)/float(0x40000000);	}

	void			clear		();
	void			propagade	();
	u32			rasterize	(occTri* T);
	BOOL			test		(float x0, float y0, float x1, float y1, float z);
	
	occTri**		get_frame	()			{ return &(bufFrame[0][0]);	}
	float*			get_depth	()			{ return &(bufDepth[0][0]);	}
	int*			get_depth_level	(int level)	
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

ENGINE_API	extern occRasterizer	Raster;

#endif // !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
