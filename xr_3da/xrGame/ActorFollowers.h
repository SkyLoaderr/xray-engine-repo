#pragma once

class CUIFollowerPanel;
class CInventoryOwner;

class CActorFollowerMngr
{
	typedef CInventoryOwner			FOLLOWER_T;
	typedef xr_vector<FOLLOWER_T*>	FOLLOWER_V;
	typedef FOLLOWER_V::iterator	FOLLOWER_IT;

	FOLLOWER_V						m_followers;
	CUIFollowerPanel*				m_uiPanel;
public:
			CActorFollowerMngr		();
			~CActorFollowerMngr		();
	void	AddFollower				(FOLLOWER_T* f);
	void	RemoveFollower			(FOLLOWER_T* f);
};


