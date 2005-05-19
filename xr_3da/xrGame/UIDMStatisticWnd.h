#ifndef __XR_UIDMStatisticWnd_H__
#define __XR_UIDMStatisticWnd_H__
#pragma once

#include "ui/UIStatsWnd.h"
#include "game_cl_base.h"

class CUIDMStatisticWnd:public CUIStatsWnd
{
	typedef CUIStatsWnd inherited;

protected:
//	DEFINE_VECTOR	(LPVOID,ItemVec,ItemIt);
//	ItemVec			items;
	bool					SetItemData		(Weapon_Statistic* pWS, CUIStatsListItem *pItem);
	virtual u32				GetItemCount	()	{return UIStatsList.GetSize();};
	virtual	CUIStatsListItem*		GetItem			(int index)
	{
		return (smart_cast<CUIStatsListItem*> (UIStatsList.GetItem(index)));
	};
public:
	CUIDMStatisticWnd					();
	~CUIDMStatisticWnd					();

	virtual void Update				();
	virtual void Draw();

};

#endif //__XR_UIDMStatisticWnd_H__
