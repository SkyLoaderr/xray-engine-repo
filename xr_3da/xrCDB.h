#ifndef XRCDB_H
#define XRCDB_H

#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRCDB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRCDB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef XRCDB_EXPORTS
#define XRCDB_API __declspec(dllexport)
#else
#define XRCDB_API __declspec(dllimport)
#endif

// forward declarations
class CFrustum;
namespace Opcode {
	class OPCODE_Model;
	class AABBNoLeafNode;
};

#pragma pack(push,4)
namespace CDB
{
	// Triangle
	class XRCDB_API TRI
	{
	public:
		Fvector*		verts	[3];	// 3*4 = 12b
		TRI*			adj		[3];	// 3*4 = 12b	(24b)
		WORD			material;		// 2b			(26b)
		WORD			sector;			// 2b			(28b)
		DWORD			dummy;			// 4b			(32b)

	public:		
		IC Fvector&		V(int id)	{ return *(verts[id]); }
		
		IC DWORD*		IDverts()	{ return (DWORD*)	verts;	}
		IC DWORD*		IDadj()		{ return (DWORD*)	adj;	}
		void			convert_I2P	(Fvector* pBaseV, TRI* pBaseTri);
		void			convert_P2I	(Fvector* pBaseV, TRI* pBaseTri);
	};

	// Model definition
	class XRCDB_API MODEL
	{
		friend class COLLIDER;	
	private:
		HANDLE					hHeap;
		Opcode::OPCODE_Model*	tree;

		// tris
		TRI*			tris;
		int				tris_count;
		Fvector*		verts;
		int				verts_count;
	public:
		MODEL();
		~MODEL();

		IC TRI*			get_tris	()	{ return tris;	}

		virtual void	setheap		(HANDLE H);
		virtual DWORD	build		(Fvector* V, int Vcnt, TRI* T, int Tcnt);
		virtual DWORD	memory		();
	};

	// Collider result
	class XRCDB_API RESULT
	{
	public:
		int				id;
		float			range;
		float			u,v;
	};

	// Collider Options
	enum {
		OPT_CULL		= (1<<0),
		OPT_ONLYFIRST	= (1<<1),
		OPT_ONLYNEAREST	= (1<<2),
		OPT_FULL_TEST   = (1<<3)		// for box & frustum queries - enable class III test(s)
	};

	// Collider itself
	class XRCDB_API COLLIDER 
	{
		// Ray data and methods
		DWORD			ray_mode;
		DWORD			box_mode;
		DWORD			frustum_mode;

		// Result management
		RESULT*			rd_ptr;
		int				rd_count;
		int				rd_size;
	public:
		COLLIDER		();
		~COLLIDER		();
		
		IC void			ray_options		(DWORD f)	{	ray_mode = f;		}
		void			ray_query		(const MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);
		
		IC void			box_options		(DWORD f)	{	box_mode = f;		}
		void			box_query		(const MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
		
		IC void			frustum_options	(DWORD f)	{	frustum_mode = f;	}
		void			frustum_query	(const MODEL *m_def, const CFrustum& F);
		
		IC RESULT*		r_begin			()	{	return rd_ptr;				};
		IC RESULT*		r_end			()	{	return rd_ptr + rd_count;	};
		RESULT&			r_add			();
		void			r_free			();
		IC int			r_count			()	{	return rd_count;			};

		IC void			r_clear			()	{	rd_count = 0;				};
	};

	const DWORD			err_ok			= 0;
	const DWORD			err_memory_0	= 1;
	const DWORD			err_memory_1	= 2;
	const DWORD			err_memory_2	= 3;
	const DWORD			err_build		= 4;
};

extern "C" 
{
	XRCDB_API	void*	__cdecl		cdb_model_create	();
	XRCDB_API	void	__cdecl		cdb_model_destroy	(void*);
	XRCDB_API	DWORD	__cdecl		cdb_model_build		(CDB::MODEL *m_def, Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt);
	XRCDB_API	void*	__cdecl		cdb_collider_create	();
	XRCDB_API	void	__cdecl		cdb_collider_destroy(void*);
	XRCDB_API	void	__cdecl		cdb_query_ray		(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);
	XRCDB_API	void	__cdecl		cdb_query_box		(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
	XRCDB_API	void	__cdecl		cdb_query_frustum	(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const CFrustum& F);
};

#pragma pack(pop)	
#endif