#ifndef __XR_UIGAMECUSTOM_H__
#define __XR_UIGAMECUSTOM_H__
#pragma once


//����� ������������ �������
#include "ui\\UIDialogWnd.h"

// refs
class CUI;


class CUIGameCustom
{
protected:
	u32					uFlags;
	CUI*				m_Parent;
public:
	enum{
		flShowFragList	= (1<<1),

		fl_force_dword	= u32(-1)
	};
	void				SetFlag				(u32 mask, BOOL flag){if (flag) uFlags|=mask; else uFlags&=~mask; }
	void				InvertFlag			(u32 mask){if (uFlags&mask) uFlags&=~mask; else uFlags|=mask; }
	BOOL				GetFlag				(u32 mask){return uFlags&mask;}
public:
						CUIGameCustom		(CUI* parent){uFlags=0;m_Parent=parent;m_pUserMenu=NULL;}
	virtual				~CUIGameCustom		(){}
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);
	virtual bool		IR_OnMouseMove			(int dx, int dy);


	//��� ��������� � ������� �������������� ����
	virtual void StartStopMenu(CUIDialogWnd* pDialog);
	
	//������� ���� ������������ ���������� �� ������
	//NULL ���� ������ ������ ���
	CUIDialogWnd* m_pUserMenu;
};

//by Dandy
#include "ui.h"

#endif // __XR_UIGAMECUSTOM_H__
