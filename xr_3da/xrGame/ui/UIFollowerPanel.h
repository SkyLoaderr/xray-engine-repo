#pragma once
#include "UIWindow.h"
class CAI_Stalker;
class CUIFollowerPanel :public CUIWindow
{
public:
	virtual					~CUIFollowerPanel();
	void	Init			(Frect rect);
	void	AddFollower		(CAI_Stalker* f);
	void	RemoveFollower	(CAI_Stalker* f);
};
