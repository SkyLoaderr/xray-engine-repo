//////////////////////////////////////////////////////////////////////
// HudItem.h: класс предок дл€ всех предметов имеющих
//			  собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

class CPhysicItem;
class CWeaponHUD;
class NET_Packet;
struct HUD_SOUND;

class CHudItem {
protected: //чтоб нельз€ было вызвать на пр€мую
	CHudItem(void);
	virtual ~CHudItem(void);
	virtual DLL_Pure*_construct			();
public:
	virtual void	Load		(LPCSTR section);
	virtual CHudItem*cast_hud_item		()	{return this;}


		   void		PlaySound	(HUD_SOUND& snd,
								 const Fvector& position);
										
	///////////////////////////////////////////////
	// общие функции HUD
	///////////////////////////////////////////////

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()		const	{   return m_bPending;}
	virtual void	StopHUDSounds		()				{};
	
	//дл€ предачи команд владельцем
	virtual bool	Action				(s32 cmd, u32 flags);

	
	// Events/States
	u32				STATE, NEXT_STATE;
	IC		u32		State				() const
	{
		return		(STATE);
	}
	//посылка сообщени€ на сервер о смене состо€ни€ оружи€ 
	virtual void	SwitchState			(u32 S);
	//прием сообщени€ с сервера и его обработка
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	();
	
	virtual	BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();

	//запуск анимации, дл€ переключени€ между актерами в игре
	virtual void	StartIdleAnim		() {};

	
	//активаци€ и деактивации вещи как активной в интерфейсе
	virtual bool	Activate			();
	virtual void	Deactivate			();
	
	//инициализаци€ если вещь в активном слоте или спр€тана на OnH_B_Chield
	virtual void	OnActiveItem		() {};
	virtual void	OnHiddenItem		() {};

	//дл€ завершени€ анимации
	virtual void	OnAnimationEnd		();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();
	
	//просчет инерции дл€ HUD 
	virtual void	UpdateHudInertion		(Fmatrix& hud_trans, float actor_yaw, float actor_pitch);
	//просчет дополнительных вычислений (переопредел€етс€ в потомках)
	virtual void	UpdateHudAdditonal		(Fmatrix&);


	virtual	void	UpdateXForm			() = 0;

	CWeaponHUD*		GetHUD				() {return m_pHUD;}

protected:
	//TRUE - оружие зан€то, выполнением некоторого действи€
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	shared_str				hud_sect;
	bool					m_bRenderHud;

	//врем€ нахождени€ в текущем состо€нии
	u32						m_dwStateTime;

	//кадры момента пересчета XFORM и FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;

	//есть ли инерци€ у оружи€
	bool m_bInertionEnable;
	//вкл/выкл инерции (временное, с плавным возвращением оружи€ в состо€ние без инерции)
	void StartHudInertion();
	void StopHudInertion();
private:
	bool m_bInertionOn;

protected:
	u32				m_animation_slot;
public:
	IC		u32		animation_slot			()	{	return m_animation_slot;}

private:
	CPhysicItem		*m_object;
	CInventoryItem	*m_item;

public:
	IC		CPhysicItem	&object	() const
	{
		VERIFY		(m_object);
		return		(*m_object);
	}

	IC		CInventoryItem	&item	() const
	{
		VERIFY		(m_item);
		return		(*m_item);
	}

	virtual void	on_renderable_Render	() = 0;
};
