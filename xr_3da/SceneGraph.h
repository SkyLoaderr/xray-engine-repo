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
	struct _LineItem	{
		Fvector			P1;
		Fvector			P2;
		float			w;
		DWORD			C;
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
	typedef vector<_LineItem>				mapLineItem;

	// Top level
	typedef FixedMAP<Shader*,mapNormalItem>	mapNormal_T;
	typedef mapNormal_T::TNode				mapNormal_Node;

	typedef FixedMAP<Shader*,mapMatrixItem>	mapMatrix_T;
	typedef mapMatrix_T::TNode				mapMatrix_Node;

	typedef FixedMAP<float,_MatrixItem>		mapSorted_T;
	typedef mapSorted_T::TNode				mapSorted_Node;

	typedef FixedMAP<Shader*,mapLineItem>	mapLine_T;
	typedef mapLine_T::TNode				mapLine_Node;

	typedef CList<_PatchItem>				vecPatches_T;
};
