// occRasterizer.h: interface for the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
#define AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_
#pragma once

const int	occ_dim0	= 64;
const int	occ_dim1	= 32;
const int	occ_dim2	= 16;
const int	occ_dim3	= 8;

class occTri
{
public:	
	occTri*		adjacent[3];
};

class occRasterizer  
{
private:
	occTri*		bufFrame	[occ_dim0][occ_dim0];
	float		bufDepth0	[occ_dim0][occ_dim0];
	float		bufDepth1	[occ_dim1][occ_dim1];
	float		bufDepth2	[occ_dim2][occ_dim2];
	float		bufDepth2	[occ_dim3][occ_dim3];
public:
	occRasterizer	();
	~occRasterizer	();
};

#endif // !defined(AFX_OCCRASTERIZER_H__3739AF97_E2A7_4740_B8C2_6EEB0F8CB66D__INCLUDED_)
