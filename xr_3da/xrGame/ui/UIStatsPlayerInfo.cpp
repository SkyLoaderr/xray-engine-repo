#include "StdAfx.h"

#include "UIStatsPlayerInfo.h"
#include "UIStatic.h"
#include "../game_cl_base.h"

//#define UPD_INFO(x)

CUIStatsPlayerInfo::CUIStatsPlayerInfo(){

}

CUIStatsPlayerInfo::~CUIStatsPlayerInfo(){
    xr_vector<FIELD>::iterator it = m_fields.begin();
    
	for	(;it!=m_fields.end(); it++)
		 xr_delete((*it).wnd);
}

void CUIStatsPlayerInfo::SetInfo(game_PlayerState* pInfo){
	m_pPlayerInfo = pInfo;
}

void CUIStatsPlayerInfo::Update(){
	if (!m_pPlayerInfo)
		return;

	xr_vector<FIELD>::iterator it = m_fields.begin();
    
	for	(;it!=m_fields.end(); it++){
		FIELD& f = *it;
		f.wnd->SetText(GetInfoByID(*f.name));
	}
	m_pPlayerInfo = NULL;
}

void CUIStatsPlayerInfo::AddField(shared_str& name, float len){
	FIELD f;

	f.name = name;
	f.wnd = xr_new<CUIStatic>();

	if (m_fields.empty())
		f.wnd->Init(0,0,len,this->GetHeight());
	else
		f.wnd->Init(m_fields.back().wnd->GetWndRect().right,0,len,this->GetHeight());
	m_fields.push_back(f);
	AttachChild(f.wnd);
}

const char* CUIStatsPlayerInfo::GetInfoByID(const char* id){
	static string64 ans;

	if (0 == xr_strcmp(id,"name"))
		strcpy(ans,m_pPlayerInfo->name);
	else if (0 == xr_strcmp(id,"frags"))
		sprintf(ans,"%d",(int)m_pPlayerInfo->kills);
	else if (0 == xr_strcmp(id,"deaths"))
		sprintf(ans,"%d",(int)m_pPlayerInfo->deaths);
	else if (0 == xr_strcmp(id,"ping"))
		sprintf(ans,"%d",(int)m_pPlayerInfo->ping);
	else if (0 == xr_strcmp(id,"artefacts"))
		sprintf(ans,"%d",(int)m_pPlayerInfo->af_count);
	else if (0 == xr_strcmp(id,"rank"))
		sprintf(ans,"%d",(int)m_pPlayerInfo->rank);
	else
		R_ASSERT(false);

    return ans;
}