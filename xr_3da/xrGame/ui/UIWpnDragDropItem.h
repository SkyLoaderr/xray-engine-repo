// UIWpnDragDropItem.h: ����������� ����� ��������������� ��������
//						������������ ��� ������ � �����������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_WPN_DRAG_DROP_ITEM_H_
#define _UI_WPN_DRAG_DROP_ITEM_H_

#pragma once

#include "..\\weapon.h"

#include "UIDragDropItem.h"

class CUIWpnDragDropItem : public CUIDragDropItem
{
private:
	typedef CUIDragDropItem inherited;
public:
	CUIWpnDragDropItem();
	virtual ~CUIWpnDragDropItem();

	virtual void ClipperOn();
	virtual void ClipperOff();

	virtual void SetData(void* pData);
	virtual void Draw();
	virtual void SetTextureScale(float new_scale);


protected:
	CUIStaticItem m_UIStaticGrenadeLauncher;
	CUIStaticItem m_UIStaticScope;
	CUIStaticItem m_UIStaticSilencer;

	CWeapon* m_pWeapon;
};

#endif // _UI_WPN_DRAG_DROP_ITEM_H_
