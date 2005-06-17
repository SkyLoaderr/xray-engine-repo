#include "stdafx.h"
#include "ActorFollowers.h"
#include "ui/xrxmlparser.h"
#include "ui/UIFollowerPanel.h"
#include "Level.h"
#include "HUDManager.h"
#include "UIGameCustom.h"
#include "InventoryOwner.h"
#include "actor.h"

CActorFollowerMngr::CActorFollowerMngr()
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, "follower_panel.xml");

	m_uiPanel = xr_new<CUIFollowerPanel>	();
	m_uiPanel->Init							(&uiXml,"followers_panel",0);
	HUD().GetUI()->UIGame()->AddDialogToRender(m_uiPanel);
	m_uiPanel->Show							(false);
}

CActorFollowerMngr::~CActorFollowerMngr()
{
	HUD().GetUI()->UIGame()->RemoveDialogToRender(m_uiPanel);
	xr_delete(m_uiPanel);
}

void CActorFollowerMngr::AddFollower(FOLLOWER_T* f)
{
#ifdef DEBUG
	FOLLOWER_IT it = std::find(m_followers.begin(),m_followers.end(),f);
	if(it!=m_followers.end()){
		Msg("Attempt to add follower [%s] twice !!!",f->Name());
		return;
	}
#endif

	m_followers.push_back(f);
	m_uiPanel->AddFollower(f);
}

void CActorFollowerMngr::RemoveFollower(FOLLOWER_T* f)
{
	FOLLOWER_IT it = std::find(m_followers.begin(),m_followers.end(),f);
	if(it==m_followers.end()){
		Msg("Attempt to remove not registered follower [%s] !!!",f->Name());
		return;
	}
	std::remove(m_followers.begin(),m_followers.end(),f);
	m_uiPanel->RemoveFollower(f);

}

void CActor::AddFollower(CInventoryOwner* f)
{
	if(!m_followers)
		m_followers = xr_new<CActorFollowerMngr>();
	m_followers->AddFollower			(f);
}

void CActor::RemoveFollower(CInventoryOwner* f)
{
	m_followers->RemoveFollower			(f);
}

void CActor::DestroyFollowerInternal()
{
	xr_delete(m_followers);
}