#pragma once
#include "missile.h"
#include "explosive.h"
#include "../feel_touch.h"

#define SND_RIC_COUNT 5

class CGrenade :
	public CMissile,
	public CExplosive
{
	typedef CMissile inherited;
public:
	CGrenade(void);
	virtual ~CGrenade(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	
	virtual void OnH_B_Independent();
	virtual void OnH_A_Independent();
	virtual void OnH_A_Chield();
	
	virtual void OnEvent(NET_Packet& P, u16 type);
	
	virtual void OnAnimationEnd();
	virtual void UpdateCL();
	virtual void shedule_Update(u32 dt);
	virtual bool Activate();
	virtual void Deactivate();
	
	virtual void Throw();
	virtual void Destroy();

	
	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Useful() const;
	virtual u32  State(u32 state);

	virtual void						net_Import			(NET_Packet& P);					// import from server
	virtual void						net_Export			(NET_Packet& P);					// export to server
	virtual void renderable_Render() {inherited::renderable_Render();}
//	virtual void OnH_A_Chield() {inherited::OnH_A_Chield();}
	virtual void OnH_B_Chield() {inherited::OnH_B_Chield();}

	virtual bool IsPending() const {return inherited::IsPending();}
	virtual bool IsHidden()	 const {return inherited::IsHidden();}
	virtual bool IsHiding()	 const {return inherited::IsHiding();}

	virtual	void Hit(float P, Fvector &dir,	CObject* who, s16 element,
					 Fvector position_in_object_space, float impulse, 
					 ALife::EHitType hit_type = ALife::eHitTypeWound)	{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}

	virtual bool			NeedToDestroyObject	() const; 
	virtual ALife::_TIME_ID	TimePassedAfterIndependant() const;

	virtual void PutNextToSlot	();
protected:
	//время удаления оружия
	ALife::_TIME_ID			m_dwGrenadeRemoveTime;
	ALife::_TIME_ID			m_dwGrenadeIndependencyTime;
protected:
	HUD_SOUND		sndCheckout;
	ESoundTypes		m_eSoundCheckout;

protected:
	virtual	void			UpdateXForm			()		{ CMissile::UpdateXForm(); };
public:
	virtual void			make_Interpolation	();
	virtual void			PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual void			activate_physic_shell	();
	virtual void			setup_physic_shell		();
	virtual void			create_physic_shell		();
};
