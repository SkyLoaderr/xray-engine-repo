#pragma once

#include "fixedmap.h"

namespace SceneGraph
{
	// Low level
	struct _MatrixItem	{
		IVisual*		pVisual;
		Fmatrix			Matrix;				// matrix (copy)
		Fvector			vCenter;
		float			lod;
	};
	struct _LodItem		{
		float			ssa;
		IVisual*		pVisual;
	};

	// Higher level	- NORMAL
	struct mapNormalDirect
	{
		FixedMAP<float,IVisual*>			sorted;
		std::vector<IVisual*>				unsorted;
	};
	struct mapNormalItems		: public	mapNormalDirect										{	float	ssa;	};
	struct mapNormalVB			: public	FixedMAP<IDirect3DVertexBuffer9*,mapNormalItems>	{	float	ssa;	};
	struct mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalVB>					{	float	ssa;	};
	struct mapNormalStates		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalTextures>	{	float	ssa;	};
	struct mapNormalPS			: public	FixedMAP<IDirect3DPixelShader9*, mapNormalStates>	{	float	ssa;	};
	struct mapNormalVS			: public	FixedMAP<IDirect3DVertexShader9*, mapNormalPS>		{	};

	// Higher level - MATRIX
	typedef FixedMAP<float,_MatrixItem>				mapMatrixItem;

	// Top level
	typedef mapNormalVS								mapNormal_T;

	typedef FixedMAP<ShaderElement*,mapMatrixItem>	mapMatrix_T;
	typedef mapMatrix_T::TNode						mapMatrix_Node;

	typedef FixedMAP<float,_MatrixItem>				mapSorted_T;
	typedef mapSorted_T::TNode						mapSorted_Node;

	typedef FixedMAP<float,_MatrixItem>				mapHUD_T;
	typedef mapHUD_T::TNode							mapHUD_Node;

	typedef FixedMAP<float,_LodItem>				mapLOD_T;
	typedef mapLOD_T::TNode							mapLOD_Node;
};
