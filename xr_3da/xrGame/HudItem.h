//////////////////////////////////////////////////////////////////////
// HudItem.h: класс предок для всех предметов имеющих
//			  собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"
#include "HudSound.h"


class CWeaponHUD;

class CHudItem: virtual public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
protected: //чтоб нельзя было вызвать на прямую
	CHudItem(void);
	virtual ~CHudItem(void);
public:
	virtual void	Load		(LPCSTR section);


		   void		PlaySound	(HUD_SOUND& snd,
								 const Fvector& position);
										
	///////////////////////////////////////////////
	// общие функции HUD
	///////////////////////////////////////////////

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()		const	{   return m_bPending;}
	virtual void	StopHUDSounds		()				{};
	
	//для предачи команд владельцем
	virtual bool	Action				(s32 cmd, u32 flags);

	
	// Events/States
	u32				STATE, NEXT_STATE;
	IC		u32		State				() const
	{
		return		(STATE);
	}
	//посылка сообщения на сервер о смене состояния оружия 
	virtual void	SwitchState			(u32 S);
	//прием сообщения с сервера и его обработка
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	();
	
	virtual	BOOL	net_Spawn			(LPVOID DC);
	virtual void	net_Destroy			();

	//запуск анимации, для переключения между актерами в игре
	virtual void	StartIdleAnim		() {};

	
	//активация и деактивации вещи как активной в интерфейсе
	virtual bool	Activate			();
	virtual void	Deactivate			();
	
	//инициализация если вещь в активном слоте или спрятана на OnH_B_Chield
	virtual void	OnActiveItem		() {};
	virtual void	OnHiddenItem		() {};

	//для завершения анимации
	virtual void	OnAnimationEnd		();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();
	virtual	void	UpdateXForm			() = 0;

	CWeaponHUD*		GetHUD				() {return m_pHUD;}

protected:
	//TRUE - оружие занято, выполнением некоторого действия
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	ref_str					hud_sect;
	bool					m_bRenderHud;

	//время нахождения в текущем состоянии
	u32						m_dwStateTime;

	//кадры момента пересчета XFORM и FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;

protected:
	u32				m_animation_slot;
public:
	IC		u32		animation_slot			()	{	return m_animation_slot;}
};