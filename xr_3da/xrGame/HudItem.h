//////////////////////////////////////////////////////////////////////
// HudItem.h: ����� ������ ��� ���� ��������� �������
//			  ����������� HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item.h"


class CWeaponHUD;

class CHudItem: virtual public CInventoryItem
{
private:
	typedef CInventoryItem inherited;
protected: //���� ������ ���� ������� �� ������
	CHudItem(void);
	virtual ~CHudItem(void);
public:
	void		Load				(LPCSTR section);

	IC void			SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL			GetHUDmode			()				{	return hud_mode;							}
	
	virtual bool	IsPending			()				{   return m_bPending;}
	//��� ������� ������ ����������
	virtual bool	Action				(s32 cmd, u32 flags);

	
	// Events/States
	u32				STATE, NEXT_STATE;
	//������� ��������� �� ������ � ����� ��������� ������ 
	virtual void	SwitchState			(u32 S);
	//����� ��������� � ������� � ��� ���������
	virtual void	OnStateSwitch		(u32 S);
	virtual void	OnEvent				(NET_Packet& P, u16 type);
	
	//��������� � ����������� ���� ��� �������� � ����������
	virtual bool	Activate();
	virtual void	Deactivate();

		
	//��� ���������� ��������
	virtual void	OnAnimationEnd		();

	virtual void	UpdateCL();
	virtual void	renderable_Render	();


	virtual void	Hide() = 0;
	virtual void	Show() = 0;

	virtual void	UpdateHudPosition	();

protected:
	//TRUE - ������ ������, ����������� ���������� ��������
	bool					m_bPending;

	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	ref_str					hud_sect;

	//����� ���������� � ������� ���������
	u32						m_dwStateTime;

	//����� ������� ��������� XFORM � FirePos
	u32						dwFP_Frame;
	u32						dwXF_Frame;
	u32						dwHudUpdate_Frame;

};