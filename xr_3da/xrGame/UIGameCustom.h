#ifndef __XR_UIGAMECUSTOM_H__
#define __XR_UIGAMECUSTOM_H__
#pragma once


//����� ������������ �������
#include "ui/UIDialogWnd.h"

// refs
class CUI;
class CTeamBaseZone;


class CUIGameCustom :public DLL_Pure
{
protected:
	u32					uFlags;
	CUI*				m_Parent;
public:
	enum{
		flShowFragList	= (1<<1),

		fl_force_dword	= u32(-1)
	};
	void				SetFlag					(u32 mask, BOOL flag){if (flag) uFlags|=mask; else uFlags&=~mask; }
	void				InvertFlag				(u32 mask){if (uFlags&mask) uFlags&=~mask; else uFlags|=mask; }
	BOOL				GetFlag					(u32 mask){return uFlags&mask;}
	
public:
						CUIGameCustom			(){m_Parent=NULL;uFlags=0;m_pUserMenu=NULL;}
	virtual				~CUIGameCustom			(){}

			void		SetUI					(CUI* parent){m_Parent=parent;}
	virtual	void		Init					()	{};
	
	virtual void		Render					();
	virtual void		OnFrame					();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);
	virtual bool		IR_OnMouseMove			(int dx, int dy);

	virtual void		OnBuyMenu_Ok			()	{};
	virtual void		OnBuyMenu_Cancel		()	{};

	virtual void		OnSkinMenu_Ok			()	{};
	virtual void		OnSkinMenu_Cancel		()	{};

	virtual void		OnObjectEnterTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)	{};
	virtual void		OnObjectLeaveTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone)	{};

	virtual void		OnTeamSelect			(int Result)	{};
	virtual bool		CanBeReady				()	{ return true; };

	virtual CUIDialogWnd*	GetBuyWnd			()	{ return NULL; };


	//��� ��������� � ������� �������������� ����
	virtual void		StartStopMenu			(CUIDialogWnd* pDialog);
	
	//������� ���� ������������ ���������� �� ������
	//NULL ���� ������ ������ ���
	CUIDialogWnd*								m_pUserMenu;
};

//by Dandy
#include "ui.h"

#endif // __XR_UIGAMECUSTOM_H__
