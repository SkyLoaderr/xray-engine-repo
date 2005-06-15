#pragma once

#include "UIWindow.h"
#include "UIListWnd.h"
#include "UIListItemServer.h"
#include "UIFrameWindow.h"
#include "UILabel.h"

class CUIXml;


class CServerList : public CUIWindow{
public:
	CServerList();

	virtual void Init(float x, float y, float width, float height);
			void InitFromXml(CUIXml& xml_doc, const char* path);
			void InitHeader();
			void InitSeparator();
	
	virtual void	RefreshGameSpyList	(bool Local);


protected:
	LIST_SRV_ITEM m_itemInfo;
	CUIListWnd		m_list;
	CUIFrameWindow	m_frame;
	CUILabel		m_header[6];
	CUIFrameLineWnd	m_separator[5];
};