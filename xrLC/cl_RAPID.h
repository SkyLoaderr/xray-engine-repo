#ifndef RAPID_H
#define RAPID_H

#ifdef ENGINE_BUILD
#	include "..\xr_list.h"
#else
#	ifdef XRGAME_EXPORTS
#		include "..\xr_list.h"
#	else
#		ifdef _USRDLL
#			include "..\..\xr_list.h"
#		else
//#			error Project can't include "xr_list"
#		endif
#	endif
#endif

namespace RAPID {
	enum RAPID_option
	{
		RAPID_BUILD_NUM_TRIS_PER_LEAF,
		RAPID_BUILD_MAX_LEVELS_DEEP
	};
	
	struct	tri;
	class	box;
	class   ray;
	class	bbox;
	
	class Model
	{
		friend class XRCollide;
	public:
		// boxes
		box *b;
		int num_boxes_alloced;
		
		// tris
		tri *tris;
		int num_tris;
		int *tri_index;

		// vertices
		Fvector* verts;
		int	num_verts;
		
		// options/state
		int		build_opt_max_levels_deep;
		int		build_opt_num_tris_per_leaf;
		
		BOOL	build_hierarchy();
		
	public:
		// These are everything the client needs to use RAPID.
		Model();
		~Model();
		
		void	BuildModel		( Fvector* V, int Vcnt, tri* T, int Tcnt);
		DWORD	MemoryUsage		();
		
		void	SetOption		( RAPID_option opt, int val );
		void	UnsetOption		( RAPID_option opt );
		
		tri*	GetTris			() { return tris;  }
	};
	
	/****************************************************************************/
	const float MAX_RAYPICK_RANGE=500;
	
	struct collision_pair
	{
		int id1;
		int id2;
	};
	
	struct bbox_collide
	{
		int id;
	};
	
	struct raypick_info
	{
		int		id;
		float	range;
		float	u,v;
		Fvector p[3];
	};
	
	//------------------------------------------------------------------------------
	// ray
#define RAY_CULL		(1<<0)	// отсечение по нормали
#define RAY_ONLYFIRST	(1<<1)	// первый попавшийся треугольник НЕ БЛИЖАЙШИЙ!!!
								// для ближайшего полный тест, а в min_raypick_id 
								// индекс ближайшего теста в RayContact
#define RAY_ONLYNEAREST	(1<<2)	// спец. оптимизация, если надо получить только ОДИН,
								// БЛИЖАЙШИЙ треугольник

// model collide
#define CL_ONLYFIRST	(1<<0)	// первый попавшийся треугольник
	
// bbox collide
#define BBOX_CULL		(1<<0)	// отсечение по нормали
#define BBOX_TRITEST	(1<<1)	// точный тест по выбранным треугольникам BOX<->TRI
#define BBOX_ONLYFIRST	(1<<2)	// первый попавшийся треугольник НЕ БЛИЖАЙШИЙ!!!

	//------------------------------------------------------------------------------
	// X-Ray Collision class
	//------------------------------------------------------------------------------
	class ENGINE_API XRCollide
	{
		bool		Intersect_RayTri	(const ray&,Fvector**,raypick_info& rp_inf);
		bool		Intersect_RayTri	(const ray&,Fvector*,raypick_info& rp_inf);
		bool		Intersect_BBoxTri	(const bbox&,Fvector**);
		
		// internal use
		Fmatrix33   XR_mR;
		Fvector     XR_mT;
		float       XR_ms;

		Fvector		rmodel_C,rmodel_D;
		float		rmodel_range,rmodel_range_sq;
		Fmatrix*	rmodel_L2W;
		
		const Model *model1;
		const Model *model2;
		
		int			tri_contact			(Fvector* P, Fvector** Q);
		int         obb_disjoint		(const Fmatrix33& B, const Fvector& T, const Fvector& a, const Fvector& b);
		// raypick
		void        add_raypick			(const raypick_info& rp_inf);
		void        raypick_fast		(const box *B, const Fvector& C, const Fvector& D);
		void        raypick_fast_nearest(const box *B, const Fvector& C, const Fvector& D);
		// collide model
		void		add_collision		(int id1, int id2);
		void		tri_contact			(const box *b1, const box *b2);
		void        collide_recursive	(const box *b1, const box *b2, const Fmatrix33& R, const Fvector& T, float s);
		// collide bbox
		void		add_bboxcollide		(int id);
		void        bbox_contact		(const box *b, const bbox *bb);
		void		bbox_recursive		(const box *b, const bbox *bb, const Fmatrix33& R, const Fvector& T, float s);
	protected:
		// flags
		DWORD		ray_flags;
		DWORD		collide_flags;
		DWORD		bbox_flags;
		int			min_raypick_id;
	public:
		// last collide info
		CList<collision_pair>   ModelContact;
		CList<raypick_info>     RayContact;
		CList<bbox_collide>		BBoxContact;
	public:
		XRCollide	();
		~XRCollide	();
		
		IC const raypick_info*	GetMinRayPickInfo(){return (min_raypick_id>=0)?&(RayContact[min_raypick_id]):NULL;}
		IC bool GetMinRayPickDistance(float& d){
			if (min_raypick_id>=0){
				VERIFY(min_raypick_id<(int)RayContact.size());
				d = RayContact[min_raypick_id].range;
				return true;
			}else return false;
		}
		
		IC DWORD  GetModelContactCount()  {return ModelContact.size();}
		IC DWORD  GetRayContactCount  ()  {return RayContact.size();}
		IC DWORD  GetBBoxContactCount ()  {return BBoxContact.size();}
		
		void Collide	(const Fmatrix33& R1, const Fvector& T1, float s1, const Model *o1, const Fmatrix33& R2, const Fvector& T2, float s2, const Model *o2);
		void Collide	(const Fmatrix33& R1, const Fvector& T1, const Model *o1, const Fmatrix33& R2, const Fvector& T2, const Model *o2)
		{	Collide(R1,T1,1.f,o1,R2,T2,1.f,o2); }
		void RayPick	(const Fmatrix* parent, const Model *o, const Fvector& start,  const Fvector& dir, float max_range = MAX_RAYPICK_RANGE);
		void BBoxCollide(const Fmatrix& parent, const Model *o, const Fmatrix& P_bbox, const Fvector& bbox_center, const Fvector& bbox_radius);
		void BBoxCollide(const Fmatrix& parent, const Model *o, const Fmatrix& P_bbox, const Fbox& s_box)
		{
			Fvector bbox_center, bbox_radius;
			s_box.getcenter(bbox_center); 
			s_box.getsize(bbox_radius); 
			bbox_radius.mul(.5f);
			BBoxCollide(parent,o,P_bbox,bbox_center,bbox_radius);
		}
		
		void	RayMode		(DWORD m)	{ray_flags = m;		}
		DWORD	GetRayMode	()			{return ray_flags;	}
		void	CollideMode	(DWORD m)	{collide_flags = m;	}
		void	BBoxMode	(DWORD m)	{bbox_flags = m;	}
	};
};

ENGINE_API extern RAPID::XRCollide XRC;

/****************************************************************************/
#endif
