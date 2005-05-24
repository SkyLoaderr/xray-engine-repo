#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"

class CUIXml;


class CServerList : public CUIWindow{
public:
	CServerList();

	virtual void Init(int x, int y, int width, int height);
			void InitFromXml(CUIXml& xml_doc, const char* path);


protected:
	LIST_SRV_ITEM m_itemInfo;
	CUIListWnd	m_list;
};