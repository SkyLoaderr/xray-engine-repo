//////////////////////////////////////////////////////////////////////
// HudItem.h: класс предок для всех предметов имеющих
//			  собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"


class CWeaponHUD;

//описание звуков применяемых в HUD-е 
//с дополнительными параметрами
struct HUD_SOUND
{
	void set_position(const Fvector& pos) {snd.set_position(pos);}
	
	ref_sound snd;
	float delay;	//задержка перед проигрыванием
	float volume;	//громкость
};



class CHudItem: virtual public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
protected: //чтоб нельзя было вызвать на прямую
	CHudItem(void);
	virtual ~CHudItem(void);
public:
	virtual void	Load		(LPCSTR section);

	////////////////////////////////////
	// работа со звуками
	/////////////////////////////////////

	static void		LoadSound	(LPCSTR section, LPCSTR line,
									ref_sound& hud_snd, BOOL _3D,
									int type = st_SourceType,
									float* volume = NULL,
									float* delay = NULL);
	static void		LoadSound	(LPCSTR section, LPCSTR line,
									 HUD_SOUND& hud_snd, BOOL _3D,
									 int type = st_SourceType);

	static void		DestroySound (HUD_SOUND& hud_snd);

		   void		PlaySound	(HUD_SOUND& snd,
								 const Fvector& position);
										
	///////////////////////////////////////////////
	// общие функции HUD
	///////////////////////////////////////////////

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()		const	{   return m_bPending;}
	
	//для предачи команд владельцем
	virtual bool	Action				(s32 cmd, u32 flags);

	
	// Events/States
	u32				STATE, NEXT_STATE;
	//посылка сообщения на сервер о смене состояния оружия 
	virtual void	SwitchState			(u32 S);
	//прием сообщения с сервера и его обработка
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	net_Destroy			();

	
	//активация и деактивации вещи как активной в интерфейсе
	virtual bool	Activate			();
	virtual void	Deactivate			();
	

	//для завершения анимации
	virtual void	OnAnimationEnd		();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();

protected:
	//TRUE - оружие занято, выполнением некоторого действия
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	ref_str					hud_sect;

	//время нахождения в текущем состоянии
	u32						m_dwStateTime;

	//кадры момента пересчета XFORM и FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;
	u32						dwHudUpdate_Frame;
};