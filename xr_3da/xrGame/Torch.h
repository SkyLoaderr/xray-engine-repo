#pragma once
#include "attachable_item.h"
#include "night_vision_effector.h"
#include "hudsound.h"

#include "ui/uistatic.h"

class CLAItem;
class CMonsterEffector;

class CTorch : public CAttachableItem 
{
private:
    typedef	CAttachableItem	inherited;
protected:
	float			fBrightness;
	CLAItem*		lanim;
	float			time2hide;

	u16				guid_bone;
	ref_str			light_trace_bone;

	Fvector			m_pos;
	IRender_Light*	light_render;
	IRender_Glow*	glow_render;
	Fvector			m_focus;

public:
					CTorch				(void);
	virtual			~CTorch				(void);

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();
	virtual void	reload				(LPCSTR section);
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	net_Destroy			();
	virtual void	net_Export			(NET_Packet& P);				// export to server
	virtual void	net_Import			(NET_Packet& P);				// import from server

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

			void	Switch				();
			void	Switch				(bool light_on);

//////////////////////////////////////////////////////////////////////////
//Night vision
public:
			void	SwitchNightVision		  ();
			void	SwitchNightVision		  (bool light_on);
			void	UpdateSwitchNightVision   ();
			float	NightVisionBattery		  ();
protected:
	bool					m_bNightVisionEnabled;
	bool					m_bNightVisionOn;
	CNightVisionEffector	m_NightVisionEffector;

	HUD_SOUND				m_NightVisionOnSnd;
	HUD_SOUND				m_NightVisionOffSnd;
	HUD_SOUND				m_NightVisionIdleSnd;
	CUIStaticItem			m_NightVisionTexture;

	float					m_NightVisionRechargeTime;
	float					m_NightVisionRechargeTimeMin;
	float					m_NightVisionDischargeTime;
	float					m_NightVisionChargeTime;

public:

	virtual bool			use_parent_ai_locations	() const
	{
		return				(!H_Parent());
	}
	virtual void	create_physic_shell		();
	virtual void	activate_physic_shell	();
	virtual void	setup_physic_shell		();
};