#pragma once

#include "fixedmap.h"

namespace SceneGraph
{
	// Low level
	struct _MatrixItem	{
		IVisual*		pVisual;
		Fmatrix			Matrix;				// matrix (copy)
		Fvector3		vCenter;
	};
	struct _LodItem		{
		float			ssa;
		IVisual*		pVisual;
	};

	// Higher level	- NORMAL
	struct	mapNormalDirect
	{
		FixedMAP<float,IVisual*>			sorted;
		std::vector<IVisual*>				unsorted;
	};
	struct	mapNormalItems		: public	mapNormalDirect										{	float	ssa;	};
	struct	mapNormalVB			: public	FixedMAP<IDirect3DVertexBuffer9*,mapNormalItems>	{	float	ssa;	};
	struct	mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalVB>					{	float	ssa;	};
	struct	mapNormalStates		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalTextures>	{	float	ssa;	};
	struct	mapNormalCS			: public	FixedMAP<R_constant_table*,mapNormalStates>			{	float	ssa;	};
	struct	mapNormalPS			: public	FixedMAP<IDirect3DPixelShader9*, mapNormalCS>		{	float	ssa;	};
	struct	mapNormalVS			: public	FixedMAP<IDirect3DVertexShader9*, mapNormalPS>		{	};
	typedef mapNormalVS			mapNormal_T;

	// Higher level	- NORMAL
	struct	mapMatrixDirect
	{
		FixedMAP<float,_MatrixItem>			sorted;
		std::vector<_MatrixItem>			unsorted;
	};
	struct	mapMatrixItems		: public	mapNormalDirect										{	float	ssa;	};
	struct	mapMatrixVB			: public	FixedMAP<IDirect3DVertexBuffer9*,mapNormalItems>	{	float	ssa;	};
	struct	mapMatrixTextures	: public	FixedMAP<STextureList*,mapNormalVB>					{	float	ssa;	};
	struct	mapMatrixStates		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalTextures>	{	float	ssa;	};
	struct	mapMatrixCS			: public	FixedMAP<R_constant_table*,mapNormalStates>			{	float	ssa;	};
	struct	mapMatrixPS			: public	FixedMAP<IDirect3DPixelShader9*, mapNormalCS>		{	float	ssa;	};
	struct	mapMatrixVS			: public	FixedMAP<IDirect3DVertexShader9*, mapNormalPS>		{	};
	typedef mapMatrixVS			mapMatrix_T;

	// Top level
	typedef FixedMAP<float,_MatrixItem>				mapSorted_T;
	typedef mapSorted_T::TNode						mapSorted_Node;

	typedef FixedMAP<float,_MatrixItem>				mapHUD_T;
	typedef mapHUD_T::TNode							mapHUD_Node;

	typedef FixedMAP<float,_LodItem>				mapLOD_T;
	typedef mapLOD_T::TNode							mapLOD_Node;
};
