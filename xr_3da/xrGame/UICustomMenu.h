#ifndef __XR_UICUSTOMMENU_H__
#define __XR_UICUSTOMMENU_H__
#pragma once

DEFINE_VECTOR			(CCustomMenuItem*,MIVec,MIIt);
typedef void 			(*OnExecuteEvent)		(CCustomMenuItem* sender);
class CCustomMenuItem{
public:
	LPSTR				caption;
	int					tag;
	MIVec				items;
	OnExecuteEvent		OnExecute;
	CCustomMenuItem*	m_Parent;
public:
	CCustomMenuItem		(CCustomMenuItem* parent, LPCSTR text, int t, OnExecuteEvent event=0)
	{
		m_Parent		= parent;
		caption			= xr_strdup(text);
		tag				= t;
		OnExecute		= event;
	}
	~CCustomMenuItem	()
	{
		xr_free			(caption);
		for (MIIt it=items.begin(); it!=items.end(); it++)
			_DELETE		(*it);
	}
	void				AppendItem(CMenuItem* I)
	{
		items.push_back	(I);
	}
	void				OnItemDraw(CGameFont* F, int num, int col)
	{
		switch(col){
		case 0: F->OutNext		("%d. %s",num,caption);	break;
		case 1: if (!IsMenu())	F->OutNext	("%d",tag);	break;
		default: THROW;
		}
	}
	IC BOOL				IsMenu				()			{return (tag==-1);}
	IC CCustomMenuItem*	GetItem				(int id)	
	{
		id--;
		if (-1==id) return m_Parent;
		if (id<(int)(items.size())) return items[id];
		return 0;
	}
	IC void				Execute				()			{if (OnExecute) OnExecute(this);}
};
void					ParseMenu			(CInifile* ini, CMenuItem* root, LPCSTR sect);
