#pragma once
#ifndef CPHOBJECT
#define CPHOBJECT
#include "../ispatial.h"
#include "PHItemList.h"
#include "PHIsland.h"


DEFINE_VECTOR(ISpatial*,qResultVec,qResultIt)
class CPHObject;
class CPHUpdateObject;
class CPHMoveStorage;
typedef void CollideCallback(CPHObject* obj1,CPHObject* obj2, dGeomID o1, dGeomID o2);

class CPHObject :
	public ISpatial 
{
	DECLARE_PHLIST_ITEM(CPHObject)

			Flags8	m_flags;

			enum{
					st_activated					=(1<<0),
					st_freezed						=(1<<1),
					st_dirty						=(1<<2),
					st_net_interpolation			=(1<<3),
					fl_ray_motions					=(1<<4)
				};

			CPHIsland		m_island;

public:
			enum ECastType
			{
					tpNotDefinite,
					tpShell,
					tpCharacter,
					tpStaticShell
			};
protected:
				Fvector		AABB;
protected:

	virtual		dGeomID		dSpacedGeom						()								=0;
	virtual		void		get_spatial_params				()								=0;
	virtual		void		spatial_register				()								;
				void		SetRayMotions					()								{m_flags.set(fl_ray_motions,TRUE);}
				void		SetPrefereExactIntegration		()								{m_island.SetPrefereExactIntegration();}

				void		UnsetRayMotions					()								{m_flags.set(fl_ray_motions,FALSE);}

				CPHObject*	SelfPointer						()								{return this;}
public:
				void		IslandReinit					()								{m_island.Unmerge();}
				void		IslandStep						(dReal step)					{m_island.Step(step);}
				void		MergeIsland						(CPHObject* obj)				{m_island.Merge(&obj->m_island);}
				CPHIsland&	Island							()								{return m_island;}
				dWorldID	DActiveWorld					()								{return m_island.DActiveWorld();}
				CPHIsland*	DActiveIsland					()								{return m_island.DActiveIsland();}
				dWorldID	DWorld							()								{return m_island.DWorld();}
	
	virtual		void		FreezeContent					()								;
	virtual		void		UnFreezeContent					()								;
	virtual		void 		EnableObject					(CPHObject* obj)				;

	virtual 	void 		PhDataUpdate					(dReal step)					=0;
	virtual 	void 		PhTune							(dReal step)					=0;
	virtual		void 		spatial_move					()								;
	virtual 	void 		InitContact						(dContact* c,bool& do_collide,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)	=0;
					

				void 		Freeze							()								;
				void 		UnFreeze						()								;
				void		NetInterpolationON				()								{m_flags.set(st_net_interpolation,TRUE);}
				void		NetInterpolationOFF				()								{m_flags.set(st_net_interpolation,TRUE);}
				bool		NetInterpolation				()								{return !!(m_flags.test(st_net_interpolation));}
				
	//virtual void StepFrameUpdate(dReal step)=0;


							CPHObject						()										;
			void			activate						()										;
		IC	bool			is_active						()										{return !!m_flags.test(st_activated)/*b_activated*/;}
			void			deactivate						()										;
virtual		void			Collide							()										;
virtual		void			near_callback					(CPHObject* obj)						{;}
virtual		void			RMotionsQuery					(qResultVec	&res)						{;}
virtual		CPHMoveStorage*	MoveStorage						()										{return NULL;}
virtual		ECastType		CastType						(){return tpNotDefinite;}
virtual		void			vis_update_activate				()										{}
virtual		void			vis_update_deactivate			()										{}
};



class CPHUpdateObject 
{
	DECLARE_PHLIST_ITEM(CPHUpdateObject)
	bool				b_activated												;

public:
					CPHUpdateObject	()											;
	void			Activate		()											;
	void			Deactivate		()											;
	virtual void	PhDataUpdate	(dReal step)								=0;
	virtual void	PhTune			(dReal step)								=0;
};

DEFINE_PHITEM_LIST(CPHObject,PH_OBJECT_STORAGE,PH_OBJECT_I)
DEFINE_PHITEM_LIST(CPHUpdateObject,PH_UPDATE_OBJECT_STORAGE,PH_UPDATE_OBJECT_I)
#endif//CPHOBJECT