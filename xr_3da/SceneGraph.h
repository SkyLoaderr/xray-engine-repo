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

	// Higher level
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
	struct mapNormalItem 
	{
		BOOL								ssa_valid;
		float								ssa;
		mapNormalDirect						direct;
		mapNormalCached						cached;
	};
	typedef FixedMAP<float,_MatrixItem>		mapMatrixItem;

	// Shader sorting
	typedef FixedMAP<SConstantList*,mapNormalItem>			mapNormal_Constants;
	typedef FixedMAP<SMatrixList*,mapNormal_Constants>		mapNormal_Matrices;
	typedef FixedMAP<STextureList*,mapNormal_Matrices>		mapNormal_Textures;
	typedef FixedMAP<DWORD,mapNormal_Textures>				mapNormal_Code;

	// Top level
	typedef mapNormal_Code					mapNormal_T		[8];

	typedef FixedMAP<Shader*,mapMatrixItem>	mapMatrix_T;
	typedef mapMatrix_T::TNode				mapMatrix_Node;

	typedef FixedMAP<float,_MatrixItem>		mapSorted_T;
	typedef mapSorted_T::TNode				mapSorted_Node;

	typedef CList<_PatchItem>				vecPatches_T;
};
