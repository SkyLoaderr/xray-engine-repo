#pragma once

#include "FixedMap.h"
#include "xr_List.h"

class ENGINE_API FCached;
class ENGINE_API CVisual;

namespace SceneGraph
{
	// Low level
	struct _MatrixItem	{
		CVisual*		pVisual;
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
		FixedMAP<float,CVisual*>			sorted;
		CList<CVisual*>						unsorted;
	};
	struct mapNormalCached
	{
		FixedMAP<float,FCached*>			sorted;
		CList<FCached*>						unsorted;
	};
	struct mapNormalItems 
	{
		float								ssa;
		mapNormalDirect						direct;
		mapNormalCached						cached;
	};
	struct mapNormalConstants	: public	FixedMAP<SConstantList*,mapNormalItems>
	{
		float								ssa;
	};
	struct mapNormalMatrices	: public	FixedMAP<SMatrixList*,mapNormalConstants>
	{
		float								ssa;
	};
	struct mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalMatrices>
	{
		float								ssa;
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
