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
	class XRCDB_API TRI					//*** 16 bytes total (was 32 :)
	{
	public:
		Fvector*		verts	[3];	// 3*4 = 12b
		union
		{
			u32			dummy;			// 4b
			struct {
				u16		material;		// 2b
				u16		sector;			// 2b
			};
		};
	public:
		IC Fvector&		V(u32 id)	{return *(verts[id]);	}

		IC u32*			IDverts()	{return (u32*)	verts;	}
		void			convert_I2P	(Fvector* pBaseV);
		void			convert_P2I	(Fvector* pBaseV);
	};

	// Build callback
	typedef		void __stdcall	build_callback	(Fvector* V, int Vcnt, TRI* T, int Tcnt, void* params);

	// Model definition
	class XRCDB_API MODEL
	{
		friend class COLLIDER;
		enum
		{
			S_READY				= 0,
			S_INIT				= 1,
			S_BUILD				= 2,
			S_forcedword		= u32(-1)
		};
	private:
		u32						status;		// 0=ready, 1=init, 2=building
		xrCriticalSection		cs;
		Opcode::OPCODE_Model*	tree;

		// tris
		TRI*					tris;
		int						tris_count;
		Fvector*				verts;
		int						verts_count;
	public:
		MODEL();
		~MODEL();

		IC TRI*					get_tris		()			{ return tris;		}
		IC int					get_tris_count	()	const	{ return tris_count;}
		IC void					syncronize		()	const
		{
			if (S_READY!=status)
			{
				Log						("! WARNING: syncronized CDB::query");
				xrCriticalSection*	C	= (xrCriticalSection*) &cs;
				C->Enter				();
				C->Leave				();
			}
		}

		static	void	__cdecl	build_thread	(void*);
		void					build_internal	(Fvector* V, int Vcnt, TRI* T, int Tcnt, build_callback* bc=NULL, void* bcp=NULL);
		void					build			(Fvector* V, int Vcnt, TRI* T, int Tcnt, build_callback* bc=NULL, void* bcp=NULL);
		u32						memory			();
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
		u32				ray_mode;
		u32				box_mode;
		u32				frustum_mode;

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

	//
	const u32 edge_open = 0xffffffff;

	class XRCDB_API Collector
	{
		xr_vector<Fvector>	verts;
		xr_vector<TRI>		faces;

		u32					VPack(Fvector& V, float eps);
	public:
		void			add_face(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			u32 e01, u32 e12, u32 e20,				// edges
			WORD material, WORD sector, u32 dummy	// misc
			);
		void			add_face_packed(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			u32 e01, u32 e12, u32 e20,				// edges
			WORD material, WORD sector, u32 dummy,	// misc
			float eps = EPS
			);
		void			calc_adjacency	();

		Fvector*		getV()	{ return &*verts.begin();	}
		size_t			getVS()	{ return verts.size();	}
		TRI*			getT()	{ return &*faces.begin();	}
		size_t			getTS() { return faces.size();	}
	};

	const u32 clpMX = 28, clpMY=16, clpMZ=28;
	class XRCDB_API CollectorPacked
	{
		typedef xr_vector<u32>		DWORDList;
		typedef DWORDList::iterator	DWORDIt;

		xr_vector<Fvector>	verts;
		xr_vector<TRI>		faces;

		Fvector				VMmin, VMscale;
		DWORDList			VM	[clpMX+1][clpMY+1][clpMZ+1];
		Fvector				VMeps;

		u32					VPack	(Fvector& V);
	public:
		CollectorPacked	(const Fbox &bb, int apx_vertices=5000, int apx_faces=5000);

		void				add_face(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			u32 e01, u32 e12, u32 e20,				// edges
			WORD material, WORD sector, u32 dummy	// misc
			);
		xr_vector<Fvector>& getV_Vec()	{ return verts;	}
		Fvector*			getV()		{ return &*verts.begin();	}
		size_t				getVS()		{ return verts.size();	}
		TRI*				getT()		{ return &*faces.begin();	}
		size_t				getTS()		{ return faces.size();	}
	};
};

#pragma pack(pop)
#endif
