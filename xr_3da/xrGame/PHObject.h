#pragma once
#ifndef CPHOBJECT
#define CPHOBJECT
#include "../ispatial.h"
#include "PHItemList.h"
class CPHObject;

class CPHUpdateObject;

typedef void CollideCallback(CPHObject* obj1,CPHObject* obj2, dGeomID o1, dGeomID o2);

class CPHObject :
	public ISpatial 
{
	DECLARE_PHLIST_ITEM(CPHObject)
				//PH_OBJECT_I m_ident;
				bool		b_activated;
				bool		b_freezed;
				bool		b_dirty;
								
protected:
				Fvector		AABB;
protected:

	virtual		dGeomID		dSpacedGeom			()								=0;
	virtual		void		get_spatial_params	()								=0;

	virtual		void		spatial_register	()								;

public:
	virtual		void		FreezeContent		()								;
	virtual		void		UnFreezeContent		()								;
	virtual		void 		EnableObject	()											;
	virtual 	void 		PhDataUpdate	(dReal step)						=0;
	virtual 	void 		PhTune			(dReal step)						=0;
	virtual		void 		spatial_move	()									;
	virtual 	void 		InitContact	(dContact* c,bool& do_collide)			=0;
	
				void 		Freeze			()									;
				void 		UnFreeze		()									;

	//virtual void StepFrameUpdate(dReal step)=0;


						CPHObject		()										;
virtual		void		Activate		()										;
		IC	bool		IsActive		()										{return b_activated;}
			void		Deactivate		()										;
virtual		void		Collide			()										;


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