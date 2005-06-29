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
#include "../encyclopedia_article_defs.h"

class CGameTask;

class CUIJobItem: public CUIListItem
{
	typedef CUIListItem inherited;

public:
					CUIJobItem				(float leftOffest = 0);
	void			Init					(float x, float y, float width, float height);
	void			SetPicture				(LPCSTR texName, const Frect &originalRect, u32 color);
	void			ScalePictureXY			(float x, float y);
	void			SetCaption				(LPCSTR caption);
	void			SetDescription			(LPCSTR description);
//	void			SetAdditionalMaterialID	(ARTICLE_ID id) { m_id = id; UIAdditionalMaterials.Show(true); }
	ARTICLE_ID		GetAdditionalMaterialID	() const ;//{ return m_id; }
	void			SetCallbackMessage		(EUIMessages msg)							{ articleTypeMsg = msg; }

	virtual void	SendMessage				(CUIWindow *pWnd, s16 msg, void *pData);
	void			SetGameTask				(CGameTask* gt, int obj_idx);//				{ m_GameTask = gt;m_TaskObjectiveIdx = obj_idx;}
//	CGameTask*		GetGameTask				()											{ return m_GameTask;}
	virtual	void	Update					();	
protected:
	CGameTask*		m_GameTask;
	int				m_TaskObjectiveIdx;
	// ��������
	CUIStatic		UIPicture;
	// ���������
	CUIStatic		UICaption;
	// ���� ���������
	CUIStatic		UIDescription;
	// ������ �� �������������� ���������
	CUIButton		UIAdditionalMaterials;
	// ID �� ������ � ��� �����
//	ARTICLE_ID		m_id;
	// �������������� ������� � ������������ �� ��������
	EUIMessages		articleTypeMsg;
};

//////////////////////////////////////////////////////////////////////////

#endif