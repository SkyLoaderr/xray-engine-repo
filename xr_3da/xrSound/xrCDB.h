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
		u32				dummy;			// 4b			(32b)

	public:
		IC Fvector&		V(int id)	{ return *(verts[id]); }

		IC u32*			IDverts()	{ return (u32*)	verts;	}
		IC u32*			IDadj()		{ return (u32*)	adj;	}
		void			convert_I2P	(Fvector* pBaseV, TRI* pBaseTri);
		void			convert_P2I	(Fvector* pBaseV, TRI* pBaseTri);
	};

	// Model definition
	class XRCDB_API MODEL
	{
		friend class COLLIDER;
	private:
		HANDLE					heapHandle;
		BOOL					heapPrivate;

		Opcode::OPCODE_Model*	tree;

		// tris
		TRI*					tris;
		int						tris_count;
		Fvector*				verts;
		int						verts_count;
	public:
		MODEL();
		~MODEL();

		IC TRI*					get_tris	()	{ return tris;	}

		virtual u32	build		(Fvector* V, int Vcnt, TRI* T, int Tcnt, BOOL bPrivateHeap=FALSE);
		virtual u32	memory		();
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
		u32			ray_mode;
		u32			box_mode;
		u32			frustum_mode;

		// Result management
		RESULT*			rd_ptr;
		int				rd_count;
		int				rd_size;
	public:
		COLLIDER		();
		~COLLIDER		();

		IC void			ray_options		(u32 f)	{	ray_mode = f;		}
		void			ray_query		(const MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);

		IC void			box_options		(u32 f)	{	box_mode = f;		}
		void			box_query		(const MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);

		IC void			frustum_options	(u32 f)	{	frustum_mode = f;	}
		void			frustum_query	(const MODEL *m_def, const CFrustum& F);

		IC RESULT*		r_begin			()	{	return rd_ptr;				};
		IC RESULT*		r_end			()	{	return rd_ptr + rd_count;	};
		RESULT&			r_add			();
		void			r_free			();
		IC int			r_count			()	{	return rd_count;			};

		IC void			r_clear			()	{	rd_count = 0;				};
	};

	const u32			err_ok			= 0;
	const u32			err_memory_0	= 1;
	const u32			err_memory_1	= 2;
	const u32			err_memory_2	= 3;
	const u32			err_build		= 4;

	//
	const u32 edge_open = 0xffffffff;

	class XRCDB_API Collector
	{
		vector<Fvector>	verts;
		vector<TRI>		faces;

		IC u32		VPack(Fvector& V, float eps)
		{
			vector<Fvector>::iterator I,E;
			I=verts.begin(); E=verts.end();
			for (;I!=E;I++) if (I->similar(V,eps)) return (I-verts.begin());
			verts.push_back(V);
			return verts.size()-1;
		}
	public:
		void			add_face(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			u32 e01, u32 e12, u32 e20,		// edges
			WORD material, WORD sector, u32 dummy	// misc
			)
		{
			TRI T;
			T.IDverts()	[0] = verts.size();
			T.IDverts()	[1] = verts.size()+1;
			T.IDverts()	[2] = verts.size()+2;
			T.IDadj()	[0]	= e01;
			T.IDadj()	[1]	= e12;
			T.IDadj()	[2]	= e20;
			T.material		= material;
			T.sector		= sector;
			T.dummy			= dummy;

			verts.push_back(v0);
			verts.push_back(v1);
			verts.push_back(v2);
			faces.push_back(T);
		}
		void			add_face_packed(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			u32 e01, u32 e12, u32 e20,		// edges
			WORD material, WORD sector, u32 dummy,// misc
			float eps = EPS
			)
		{
			TRI T;
			T.IDverts()	[0] = VPack(v0,eps);
			T.IDverts()	[1] = VPack(v1,eps);
			T.IDverts()	[2] = VPack(v2,eps);
			T.IDadj()	[0]	= e01;
			T.IDadj()	[1]	= e12;
			T.IDadj()	[2]	= e20;
			T.material		= material;
			T.sector		= sector;
			T.dummy			= dummy;
			faces.push_back(T);
		}
		void			calc_adjacency	();

		Fvector*		getV()	{ return &*verts.begin();	}
		u32				getVS()	{ return verts.size();	}
		TRI*			getT()	{ return &*faces.begin();	}
		u32				getTS() { return faces.size();	}
	};

	const u32 clpMX = 28, clpMY=16, clpMZ=28;

	class XRCDB_API CollectorPacked
	{
		typedef vector<u32>		DWORDList;
		typedef DWORDList::iterator	DWORDIt;

		vector<Fvector>	verts;
		vector<TRI>		faces;

		Fvector			VMmin, VMscale;
		DWORDList		VM[clpMX+1][clpMY+1][clpMZ+1];
		Fvector			VMeps;

		u32			VPack(Fvector& V);
	public:
		CollectorPacked	(const Fbox &bb, int apx_vertices=5000, int apx_faces=5000);

		void			add_face(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			u32 e01, u32 e12, u32 e20,		// edges
			WORD material, WORD sector, u32 dummy	// misc
			)
		{
			TRI T;
			T.IDverts()	[0] = VPack(v0);
			T.IDverts()	[1] = VPack(v1);
			T.IDverts()	[2] = VPack(v2);
			T.IDadj()	[0]	= e01;
			T.IDadj()	[1]	= e12;
			T.IDadj()	[2]	= e20;
			T.material		= material;
			T.sector		= sector;
			T.dummy			= dummy;
			faces.push_back(T);
		}
		vector<Fvector>& getV_Vec()	{ return verts;	}
		Fvector*		getV()	{ return &*verts.begin();	}
		u32				getVS()	{ return verts.size();	}
		TRI*			getT()	{ return &*faces.begin();	}
		u32				getTS() { return faces.size();	}
	};
};

extern "C"
{
	XRCDB_API	void*	__cdecl		cdb_model_create	();
	XRCDB_API	void	__cdecl		cdb_model_destroy	(void*);
	XRCDB_API	u32		__cdecl		cdb_model_build		(CDB::MODEL *m_def, Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt);
	XRCDB_API	void*	__cdecl		cdb_collider_create	();
	XRCDB_API	void	__cdecl		cdb_collider_destroy(void*);
	XRCDB_API	void	__cdecl		cdb_query_ray		(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);
	XRCDB_API	void	__cdecl		cdb_query_box		(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
	XRCDB_API	void	__cdecl		cdb_query_frustum	(const CDB::COLLIDER* C, const CDB::MODEL *m_def, const CFrustum& F);
};

#pragma pack(pop)
#endif
