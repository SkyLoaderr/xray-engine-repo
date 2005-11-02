#include "StdAfx.h"

#include "UIStatsPlayerInfo.h"
#include "UIStatic.h"
#include "../game_cl_base.h"


CUIStatsPlayerInfo::CUIStatsPlayerInfo(xr_vector<PI_FIELD_INFO>* info)
{
	m_field_info = info;

	xr_vector<PI_FIELD_INFO>&	field_info = *info;
	for (u32 i = 0; i<field_info.size(); i++)
		AddField(field_info[i].width);

	R_ASSERT(!field_info.empty());
}

CUIStatsPlayerInfo::~CUIStatsPlayerInfo(){
	for (u32 i = 0; i<m_fields.size(); i++)
		xr_delete(m_fields[i]);
}

void CUIStatsPlayerInfo::SetInfo(game_PlayerState* pInfo){
	m_pPlayerInfo = pInfo;
}

void CUIStatsPlayerInfo::Update(){
	if (!m_pPlayerInfo)
		return;

	xr_vector<PI_FIELD_INFO>&	field_info = *m_field_info;

	for (u32 i = 0; i<m_fields.size(); i++)
		m_fields[i]->SetText(GetInfoByID(*field_info[i].name));

	m_pPlayerInfo = NULL;
}

void CUIStatsPlayerInfo::AddField(float len){
	CUIStatic* wnd = xr_new<CUIStatic>();

	if (m_fields.empty())
		wnd->Init(10,0,len,this->GetHeight());		
	else
	{
		wnd->Init(m_fields.back()->GetWndRect().right,0,len,this->GetHeight());
		wnd->SetTextAlignment(CGameFont::alCenter);
	}
//	wnd->SetVTextAlignment(valCenter);
	m_fields.push_back(wnd);
	AttachChild(wnd);
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
		R_ASSERT2(false, "invalid info ID");

    return ans;
}
