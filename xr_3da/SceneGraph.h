#pragma once

#include "FixedMap.h"
#include "xr_List.h"

class ENGINE_API FCached;

namespace SceneGraph
{
	// Low level
	struct _MatrixItem	{
		FBasicVisual*	pVisual;
		Fmatrix			Matrix;				// matrix (copy)
		Fvector			vCenter;
		int				iLighting;
	};
	struct _PatchItem	{
		Shader*			S;
		Fvector			P;
		float			size;
		float			angle;
		BOOL			nearer;
	};

	// Higher level	- NORMAL
	struct mapNormalDirect
	{
		FixedMAP<float,FBasicVisual*>		sorted;
		CList<FBasicVisual*>				unsorted;
	};
	struct mapNormalCached
	{
		FixedMAP<float,FCached*>			sorted;
		CList<FCached*>						unsorted;
	};
	struct mapNormalItems 
	{
		BOOL								ssa_valid;
		float								ssa;
		mapNormalDirect						direct;
		mapNormalCached						cached;
	};
	struct mapNormalConstants
	{
		BOOL								ssa_valid;
		float								ssa;
		FixedMAP<SConstantList*,mapNormalItems>		items;
	};
	struct mapNormalMatrices
	{
		BOOL								ssa_valid;
		float								ssa;
		FixedMAP<SMatrixList*,mapNormalConstants>	items;
	};
	struct mapNormalTextures
	{
		BOOL								ssa_valid;
		float								ssa;
		FixedMAP<STextureList*,mapNormalMatrices>	items;
	};
	typedef FixedMAP<DWORD,mapNormalTextures>		mapNormalCodes;

	// Higher level - MATRIX
	typedef FixedMAP<float,_MatrixItem>		mapMatrixItem;

	// Top level
	typedef mapNormalCodes					mapNormal_T		[8];

	typedef FixedMAP<Shader*,mapMatrixItem>	mapMatrix_T;
	typedef mapMatrix_T::TNode				mapMatrix_Node;

	typedef FixedMAP<float,_MatrixItem>		mapSorted_T;
	typedef mapSorted_T::TNode				mapSorted_Node;

	typedef CList<_PatchItem>				vecPatches_T;
};
