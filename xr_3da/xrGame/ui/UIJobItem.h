//=============================================================================
//  Filename:   UIJobItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������� ����� ��� �������
//=============================================================================

#ifndef UI_JOB_ITEM_H_
#define UI_JOB_ITEM_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIListItem.h"

//////////////////////////////////////////////////////////////////////////

class CUIJobItem: public CUIListItem
{
	typedef CUIListItem inherited;

public:
					CUIJobItem				(int leftOffest = 0);
	void			Init					(int x, int y, int width, int height);
	void			SetPicture				(LPCSTR texName, const Irect &originalRect, u32 color);
	void			ScalePicture			(float value) { UIPicture.SetTextureScale(UIPicture.GetTextureScale() * value); }
	void			SetCaption				(LPCSTR caption);
	void			SetDescription			(LPCSTR description);
	void			SetAdditionalMaterialID	(int id) { m_id = id; }
	int				GetAdditionalMaterialID	() const { return m_id; }
	void			SetCallbackMessage		(EUIMessages msg) { articleTypeMsg = msg; }

	virtual void	SendMessage				(CUIWindow *pWnd, s16 msg, void *pData);

protected:
	// ��������
	CUIStatic		UIPicture;
	// ���������
	CUIStatic		UICaption;
	// ���� ���������
	CUIStatic		UIDescription;
	// ������ �� �������������� ���������
	CUIButton		UIAdditionalMaterials;
	// ID �� ������ � ��� �����
	int				m_id;
	// �������������� ������� � ������������ �� ��������
	EUIMessages		articleTypeMsg;
};

//////////////////////////////////////////////////////////////////////////

#endif