//////////////////////////////////////////////////////////////////////
// HudItem.h: ����� ������ ��� ���� ��������� �������
//			  ����������� HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"


class CWeaponHUD;

//�������� ������ ����������� � HUD-� 
//� ��������������� �����������
struct HUD_SOUND
{
	void set_position(const Fvector& pos) {snd.set_position(pos);}
	
	ref_sound snd;
	float delay;	//�������� ����� �������������
	float volume;	//���������
};



class CHudItem: virtual public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
protected: //���� ������ ���� ������� �� ������
	CHudItem(void);
	virtual ~CHudItem(void);
public:
	virtual void	Load		(LPCSTR section);

	////////////////////////////////////
	// ������ �� �������
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
	// ����� ������� HUD
	///////////////////////////////////////////////

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()		const	{   return m_bPending;}
	
	//��� ������� ������ ����������
	virtual bool	Action				(s32 cmd, u32 flags);

	
	// Events/States
	u32				STATE, NEXT_STATE;
	//������� ��������� �� ������ � ����� ��������� ������ 
	virtual void	SwitchState			(u32 S);
	//����� ��������� � ������� � ��� ���������
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	net_Destroy			();

	
	//��������� � ����������� ���� ��� �������� � ����������
	virtual bool	Activate			();
	virtual void	Deactivate			();
	

	//��� ���������� ��������
	virtual void	OnAnimationEnd		();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();

	CWeaponHUD*		GetHUD				() {return m_pHUD;}

protected:
	//TRUE - ������ ������, ����������� ���������� ��������
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	ref_str					hud_sect;
	bool					m_bRenderHud;

	//����� ���������� � ������� ���������
	u32						m_dwStateTime;

	//����� ������� ��������� XFORM � FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;
	u32						dwHudUpdate_Frame;
};