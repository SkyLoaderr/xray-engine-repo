#ifndef __XR_UIDMFRAGLIST_H__
#define __XR_UIDMFRAGLIST_H__
#pragma once

#include "ui/UIStatsWnd.h"

class CUIDMFragList:public CUIStatsWnd
{
	typedef CUIStatsWnd inherited;

protected:
	DEFINE_VECTOR	(LPVOID,ItemVec,ItemIt);
	ItemVec			items;

public:
	CUIDMFragList					();
	~CUIDMFragList					();

	virtual void Show				();

	virtual void Update				();

	virtual u32						GetItemCount	()	{return UIStatsList.GetSize();};
	virtual	CUIStatsListItem*		GetItem			(int index);


	virtual void					UpdateItemsList ();
	virtual void					UpdateItemsNum	();

//	void			Init			();
//	virtual void	OnFrame			();
};
/*

//#include "uilistbox.h"

class CUIDMFragList:public CUIListBox
{
	typedef CUIListBox inherited;
public:
					CUIDMFragList	();
	void			Init			();
	virtual void	OnFrame			();
};
*/

#endif //__XR_UIFRAGLIST_H__
