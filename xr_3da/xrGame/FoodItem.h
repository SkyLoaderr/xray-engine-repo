///////////////////////////////////////////////////////////////
// FoodItem.h
// FoodItem - еда персонажей
///////////////////////////////////////////////////////////////

#pragma once

#include "eatable_item_object.h"
#include "huditem.h"

#define FOOD_HIDDEN		0
#define FOOD_SHOWING	1
#define FOOD_IDLE		2
#define FOOD_PREPARE	3
#define FOOD_EATING		5
#define FOOD_HIDING		6
#define FOOD_PLAYING	7

class CFoodItem: 
		public CEatableItemObject,
		public CHudItem
{
protected:
	bool m_bReadyToEat;

protected:
	shared_str	m_sAnimIdle;	
	shared_str	m_sAnimShow;
	shared_str	m_sAnimHide;
	shared_str	m_sAnimPlay;
	shared_str	m_sAnimPrepare;
	shared_str	m_sAnimEat;

protected:
	//смещения для render
	Fmatrix		m_offset;

public:
						CFoodItem			();
	virtual				~CFoodItem			();
	virtual	DLL_Pure	*_construct			();

public:
	virtual CFoodItem			*cast_food_item				()			{return this;}
	virtual CHudItem			*cast_hud_item				()			{return this;}
	virtual CPhysicsShellHolder	*cast_physics_shell_holder	()			{return this;}

public:
	virtual BOOL		net_Spawn			(CSE_Abstract* DC);
	virtual void		Load				(LPCSTR section);
	virtual void		net_Destroy			();
	virtual void		shedule_Update		(u32 dt);
	virtual void		UpdateCL			();
	virtual void		renderable_Render	();
	virtual void		OnH_A_Chield		();
	virtual void		OnH_B_Independent	();
	virtual void		OnH_A_Independent	();
	virtual void		OnH_B_Chield		();
	virtual void 		OnAnimationEnd		();
	virtual u32 		GetSlot				() const;
	virtual void 		Show				();
	virtual void 		Hide				();
	virtual bool 		Action				(s32 cmd, u32 flags);
	virtual void 		OnStateSwitch		(u32 S);
	virtual void 		OnEvent				(NET_Packet& P, u16 type);
	virtual bool 		Activate			();
	virtual void 		Deactivate			();
	virtual bool 		Useful				() const;
	virtual bool 		IsPending			() const;
	virtual bool 		IsHidden			() const					{return FOOD_HIDDEN == STATE;}
	virtual bool 		IsHiding			() const					{return FOOD_HIDING == STATE;}
	virtual	void		UpdateXForm			();
	IC		bool 		ready_to_eat		() const					{return	(m_bReadyToEat);}
	virtual void		on_renderable_Render();
};
