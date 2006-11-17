#pragma once

#include	"fixedmap.h"
#include	"doug_lea_memory_allocator.h"

struct doug_lea_allocator {
	template <typename T>
	struct helper {1
		typedef xalloc<T>	result;
	};

	static	void	*alloc		(const u32 &n)	{	return dlmalloc((u32)n);	}
	template <typename T>
	static	void	dealloc		(T *&p)			{	dlfree(p);					}
};


// #define	USE_RESOURCE_DEBUGGER

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
	typedef xr_vector<_NormalItem,doug_lea_allocator::helper<_NormalItem>::result>			mapNormalDirect;
	struct	mapNormalItems		: public	mapNormalDirect										{	float	ssa;	};
	struct	mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalItems,doug_lea_allocator>				{	float	ssa;	};
	struct	mapNormalStates		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalTextures,doug_lea_allocator>	{	float	ssa;	};
	struct	mapNormalCS			: public	FixedMAP<R_constant_table*,mapNormalStates,doug_lea_allocator>			{	float	ssa;	};
	struct	mapNormalPS			: public	FixedMAP<ps_type, mapNormalCS,doug_lea_allocator>						{	float	ssa;	};
	struct	mapNormalVS			: public	FixedMAP<vs_type, mapNormalPS,doug_lea_allocator>						{	};
	typedef mapNormalVS			mapNormal_T;

	// MATRIX
	typedef xr_vector<_MatrixItem,doug_lea_allocator::helper<_MatrixItem>::result>	mapMatrixDirect;
	struct	mapMatrixItems		: public	mapMatrixDirect										{	float	ssa;	};
	struct	mapMatrixTextures	: public	FixedMAP<STextureList*,mapMatrixItems,doug_lea_allocator>				{	float	ssa;	};
	struct	mapMatrixStates		: public	FixedMAP<IDirect3DStateBlock9*,mapMatrixTextures,doug_lea_allocator>	{	float	ssa;	};
	struct	mapMatrixCS			: public	FixedMAP<R_constant_table*,mapMatrixStates,doug_lea_allocator>			{	float	ssa;	};
	struct	mapMatrixPS			: public	FixedMAP<ps_type, mapMatrixCS,doug_lea_allocator>						{	float	ssa;	};
	struct	mapMatrixVS			: public	FixedMAP<vs_type, mapMatrixPS,doug_lea_allocator>						{	};
	typedef mapMatrixVS			mapMatrix_T;

	// Top level
	typedef FixedMAP<float,_MatrixItemS,doug_lea_allocator>			mapSorted_T;
	typedef mapSorted_T::TNode						mapSorted_Node;

	typedef FixedMAP<float,_MatrixItemS,doug_lea_allocator>			mapHUD_T;
	typedef mapHUD_T::TNode							mapHUD_Node;

	typedef FixedMAP<float,_LodItem,doug_lea_allocator>				mapLOD_T;
	typedef mapLOD_T::TNode							mapLOD_Node;
};
