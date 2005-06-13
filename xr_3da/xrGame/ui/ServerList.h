#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"

class CUIXml;


class CServerList : public CUIWindow{
public:
	CServerList();

	virtual void Init(float x, float y, float width, float height);
			void InitFromXml(CUIXml& xml_doc, const char* path);
	
	virtual void	RefreshGameSpyList	(bool Local);


protected:
	LIST_SRV_ITEM m_itemInfo;
	CUIListWnd	m_list;
};