#pragma once

#include	"fixedmap.h"

#define	USE_RESOURCE_DEBUGGER

namespace	R_dsgraph
{
	// Elementary types
	struct _NormalItem	{
		float				ssa;
		IRender_Visual*		pVisual;
	};
	struct _MatrixItem	{
		float				ssa;
		IRenderable*		pObject;
		IRender_Visual*		pVisual;
		Fmatrix				Matrix;				// matrix (copy)
	};
	struct _MatrixItemS	: public _MatrixItem
	{
		ShaderElement*		se;
	};
	struct _LodItem		{
		float				ssa;
		IRender_Visual*		pVisual;
	};
#ifdef USE_RESOURCE_DEBUGGER
	typedef	ref_vs						vs_type;
	typedef	ref_ps						ps_type;
#else
	typedef	IDirect3DVertexShader9*		vs_type;
	typedef	IDirect3DPixelShader9*		ps_type;
#endif
	// NORMAL
	typedef xr_vector<_NormalItem>	mapNormalDirect;
	struct	mapNormalItems		: public	mapNormalDirect										{	float	ssa;	};
	struct	mapNormalVB			: public	FixedMAP<IDirect3DVertexBuffer9*,mapNormalItems>	{	float	ssa;	};
	struct	mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalVB>					{	float	ssa;	};
	struct	mapNormalStates		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalTextures>	{	float	ssa;	};
	struct	mapNormalCS			: public	FixedMAP<R_constant_table*,mapNormalStates>			{	float	ssa;	};
	struct	mapNormalPS			: public	FixedMAP<ps_type, mapNormalCS>						{	float	ssa;	};
	struct	mapNormalVS			: public	FixedMAP<vs_type, mapNormalPS>						{	};
	typedef mapNormalVS			mapNormal_T;

	// MATRIX
	typedef xr_vector<_MatrixItem>	mapMatrixDirect;
	struct	mapMatrixItems		: public	mapMatrixDirect										{	float	ssa;	};
	struct	mapMatrixVB			: public	FixedMAP<IDirect3DVertexBuffer9*,mapMatrixItems>	{	float	ssa;	};
	struct	mapMatrixTextures	: public	FixedMAP<STextureList*,mapMatrixVB>					{	float	ssa;	};
	struct	mapMatrixStates		: public	FixedMAP<IDirect3DStateBlock9*,mapMatrixTextures>	{	float	ssa;	};
	struct	mapMatrixCS			: public	FixedMAP<R_constant_table*,mapMatrixStates>			{	float	ssa;	};
	struct	mapMatrixPS			: public	FixedMAP<ps_type, mapMatrixCS>						{	float	ssa;	};
	struct	mapMatrixVS			: public	FixedMAP<vs_type, mapMatrixPS>						{	};
	typedef mapMatrixVS			mapMatrix_T;

	// Top level
	typedef FixedMAP<float,_MatrixItemS>			mapSorted_T;
	typedef mapSorted_T::TNode						mapSorted_Node;

	typedef FixedMAP<float,_MatrixItemS>			mapHUD_T;
	typedef mapHUD_T::TNode							mapHUD_Node;

	typedef FixedMAP<float,_LodItem>				mapLOD_T;
	typedef mapLOD_T::TNode							mapLOD_Node;
};
