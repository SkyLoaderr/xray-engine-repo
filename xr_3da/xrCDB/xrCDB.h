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
class OPCODE_Model;

#pragma pack(push,4)
namespace CDB
{
	// Allocators / Deallocators
	template <class T>
	IC T*	cl_alloc	(u32 count)
	{
		return (T*) HeapAlloc	(GetProcessHeap(),HEAP_GENERATE_EXCEPTIONS,count*sizeof(T));
	}
	template <class T>
	IC void cl_free		(T* P)
	{
		HeapFree	(GetProcessHeap(),0,P);
	}

	// Triangle
	class XRCDB_API TRI
	{
	public:
		Fvector*	verts	[3];	// 3*4 = 12b
		TRI*		adj		[3];	// 3*4 = 12b	(24b)
		WORD		material;		// 2b			(26b)
		WORD		sector;			// 2b			(28b)
		DWORD		dummy;			// 4b			(32b)

	public:		
		IC Fvector&	V(int id)		{ return *(verts[id]); }
		
		IC DWORD*	IDverts()	{ return (DWORD*)	verts;	}
		IC DWORD*	IDadj()		{ return (DWORD*)	adj;	}
		IC void		convert_I2P(Fvector* pBaseV, TRI* pBaseTri)	
		{
			DWORD*	pVertsID= (DWORD*)	verts;	// as indexed form
			verts[0] = pBaseV+pVertsID[0];
			verts[1] = pBaseV+pVertsID[1];
			verts[2] = pBaseV+pVertsID[2];
			DWORD*	pAdjID	= (DWORD*)	adj;	// as indexed form (take care about open-edges)
			adj	 [0] = (0xffffffff==pAdjID[0])?0:pBaseTri+pAdjID[0];
			adj	 [1] = (0xffffffff==pAdjID[1])?0:pBaseTri+pAdjID[1];
			adj	 [2] = (0xffffffff==pAdjID[2])?0:pBaseTri+pAdjID[2];
		}
		IC void		convert_P2I(Fvector* pBaseV, TRI* pBaseTri)	
		{
			DWORD*	pVertsID= (DWORD*)	verts;	// as indexed form
			pVertsID[0] = verts[0]-pBaseV;
			pVertsID[1] = verts[1]-pBaseV;
			pVertsID[2] = verts[2]-pBaseV;
			DWORD*	pAdjID	= (DWORD*)	adj;	// as indexed form (take care about open-edges)
			pAdjID	[0]	= (adj[0])?adj[0]-pBaseTri:0xffffffff;
			pAdjID	[1]	= (adj[1])?adj[1]-pBaseTri:0xffffffff;
			pAdjID	[2]	= (adj[2])?adj[2]-pBaseTri:0xffffffff;
		}
	};

	// Model definition
	class XRCDB_API MODEL
	{
		OPCODE_Model*	tree;
	public:
	};

	// Collider result
	class XRCDB_API RESULT
	{
	public:
		int			id;
		float		range;
		float		u,v;
	};

	// Collider Options
	enum {
		OPT_CULL		= (1<<0),
		OPT_ONLYFIRST	= (1<<1),
		OPT_ONLYNEAREST	= (1<<2)
	};

	// Collider itself
	class XRCDB_API COLLIDER 
	{
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
		
		void			r_add			(int id, float range, float u, float v);
		void			r_free			();
		IC RESULT*		r_begin			()	{	return rd_ptr;				};
		IC RESULT*		r_end			()	{	return rd_ptr + rd_count;	};
		IC int			r_count			()	{	return rd_count;			};
		IC void			r_clear			()	{	rd_count = 0;				};
	};
};
#pragma pack(pop)	
