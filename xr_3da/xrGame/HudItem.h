//////////////////////////////////////////////////////////////////////
// HudItem.h: ����� ������ ��� ���� ��������� �������
//			  ����������� HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

class CSE_Abstract;
class CPhysicItem;
class CWeaponHUD;
class NET_Packet;
struct HUD_SOUND;
class CInventoryItem;

class CHudItem {
protected: //���� ������ ���� ������� �� ������
	CHudItem(void);
	virtual ~CHudItem(void);
	virtual DLL_Pure*_construct			();
public:
	virtual void	Load		(LPCSTR section);
	virtual CHudItem*cast_hud_item		()	{return this;}


		   void		PlaySound	(HUD_SOUND& snd,
								 const Fvector& position);
										
	///////////////////////////////////////////////
	// ����� ������� HUD
	///////////////////////////////////////////////

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()		const	{   return m_bPending;}
	virtual void	StopHUDSounds		()				{};
	
	//��� ������� ������ ����������
	virtual bool	Action				(s32 cmd, u32 flags);

	
	// Events/States
	u32				STATE, NEXT_STATE;
	IC		u32		State				() const
	{
		return		(STATE);
	}
	//������� ��������� �� ������ � ����� ��������� ������ 
	virtual void	SwitchState			(u32 S);
	//����� ��������� � ������� � ��� ���������
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Chield		();
	virtual void	OnH_B_Independent	();
	virtual void	OnH_A_Independent	();
	
	virtual	BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();

	//������ ��������, ��� ������������ ����� �������� � ����
	virtual void	StartIdleAnim		() {};

	
	//��������� � ����������� ���� ��� �������� � ����������
	virtual bool	Activate			();
	virtual void	Deactivate			();
	
	//������������� ���� ���� � �������� ����� ��� �������� �� OnH_B_Chield
	virtual void	OnActiveItem		() {};
	virtual void	OnHiddenItem		() {};

	//��� ���������� ��������
	virtual void	OnAnimationEnd		();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();
	
	//������� ������� ��� HUD 
	virtual void	UpdateHudInertion		(Fmatrix& hud_trans, float actor_yaw, float actor_pitch);
	//������� �������������� ���������� (���������������� � ��������)
	virtual void	UpdateHudAdditonal		(Fmatrix&);


	virtual	void	UpdateXForm			() = 0;

	CWeaponHUD*		GetHUD				() {return m_pHUD;}

protected:
	//TRUE - ������ ������, ����������� ���������� ��������
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	shared_str				hud_sect;
	bool					m_bRenderHud;

	//����� ���������� � ������� ���������
	u32						m_dwStateTime;

	//����� ������� ��������� XFORM � FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;

	//���/���� ������� (���������, � ������� ������������ ������ � ��������� ��� �������)
	void					StartHudInertion();
	void					StopHudInertion();
private:
	bool					m_bInertionEnable;
	bool					m_bInertionAllow;
protected:
	u32						m_animation_slot;
public:
	IC		u32				animation_slot			()	{	return m_animation_slot;}

private:
	CPhysicItem				*m_object;
	CInventoryItem			*m_item;

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
