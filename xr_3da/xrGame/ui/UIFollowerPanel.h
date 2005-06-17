#pragma once
#include "UIWindow.h"
class CInventoryOwner;
class CUIXml;

class CUIFollowerPanel :public CUIWindow
{
	typedef CUIWindow		inherited;
	float					m_vIdent, m_hIdent;
	Fvector2				m_iconSize;
	void	RecalcSize		();
public:
	virtual					~CUIFollowerPanel();
	void	Init			(CUIXml* doc, LPCSTR path, int index);
	void	AddFollower		(CInventoryOwner* f);
	void	RemoveFollower	(CInventoryOwner* f);
};
