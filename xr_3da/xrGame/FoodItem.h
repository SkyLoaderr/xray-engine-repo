///////////////////////////////////////////////////////////////
// FoodItem.h
// FoodItem - еда персонажей
///////////////////////////////////////////////////////////////


#pragma once

#include "eatable_item.h"
#include "huditem.h"


#define FOOD_HIDDEN		0
#define FOOD_SHOWING	1
#define FOOD_IDLE		2
#define FOOD_PREPARE	3
#define FOOD_EATING		5
#define FOOD_HIDING		6
#define FOOD_PLAYING	7


class CFoodItem: public CEatableItem,
				 public CHudItem
{
private:
    typedef	CHudItem inherited;

public:
	CFoodItem(void);
	virtual ~CFoodItem(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();
	virtual void OnH_A_Independent	();
	virtual void OnH_B_Chield		();
	virtual void OnAnimationEnd		();

	virtual u32 GetSlot				() const;

	virtual void Show();
	virtual void Hide();

	virtual bool Action(s32 cmd, u32 flags);

	virtual void OnStateSwitch	(u32 S);

	virtual void OnEvent(NET_Packet& P, u16 type) {inherited::OnEvent(P, type);}
	virtual bool Activate() {return inherited::Activate();}
	virtual void Deactivate() {inherited::Deactivate();}

	virtual bool Useful()		const	{return CEatableItem::Useful();}
	virtual bool IsPending()	const	{return inherited::IsPending();}
	virtual bool IsHidden()		const	{return FOOD_HIDDEN == STATE;}
	virtual bool IsHiding()		const	{return FOOD_HIDING == STATE;}
	IC		bool ready_to_eat	() const {return	(m_bReadyToEat);}

	virtual	void	UpdateXForm	();

protected:
	bool m_bReadyToEat;

	ref_str m_sAnimIdle;	
	ref_str m_sAnimShow;
	ref_str m_sAnimHide;
	ref_str m_sAnimPlay;
	ref_str m_sAnimPrepare;
	ref_str m_sAnimEat;

	//смещения для render
	Fmatrix m_offset;
};