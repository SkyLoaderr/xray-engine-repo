////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager.cpp
//	Created 	: 04.11.2004
//  Modified 	: 04.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autosave_manager.h"
#include "date_time.h"
#include "ai_space.h"
#include "level.h"
#include "xrMessages.h"
#include "hudmanager.h"

extern LPCSTR alife_section;

const LPCSTR TEXT_AUTOSAVE = "Autosave...";

CAutosaveManager::CAutosaveManager			()
{
	u32							hours,minutes,seconds;
	LPCSTR						section = alife_section;

	sscanf						(pSettings->r_string(section,"autosave_interval"),"%d:%d:%d",&hours,&minutes,&seconds);
	m_autosave_interval			= (u32)generate_time(1,1,1,hours,minutes,seconds);
	m_last_autosave_time		= Device.dwTimeGlobal;

	sscanf						(pSettings->r_string(section,"delay_autosave_interval"),"%d:%d:%d",&hours,&minutes,&seconds);
	m_delay_autosave_interval	= (u32)generate_time(1,1,1,hours,minutes,seconds);

	m_not_ready_count			= 0;
	m_save_state				= eSaveStateWaitForUpdate;

	shedule.t_min				= 5000;
	shedule.t_max				= 5000;
	shedule_register			();
}

CAutosaveManager::~CAutosaveManager			()
{
	shedule_unregister			();
}

float CAutosaveManager::shedule_Scale		()
{
	return						(.5f);
}

void CAutosaveManager::OnRender				()
{
	if (m_save_state == eSaveStateWaitForUpdate)
		return;

	CGameFont					*pFont = HUD().Font().pFontStat;
	float						sz = pFont->GetSize();
	pFont->SetSize  			(32);
	pFont->SetColor 			(0x80FF0000);
	pFont->OutSet   			(Device.dwWidth/2.0f-(pFont->SizeOf(TEXT_AUTOSAVE)/2.0f),Device.dwHeight/2.0f);
	pFont->OutNext  			(TEXT_AUTOSAVE);
	pFont->OnRender 			();
	pFont->SetSize  			(sz);
	
	m_save_state				= eSaveStateWaitForSend;
}

void CAutosaveManager::shedule_Update		(u32 dt)
{
	inherited::shedule_Update	(dt);

	if (!ai().get_alife())
		return;

	if (m_save_state == eSaveStateWaitForSend) {
		NET_Packet				net_packet;
		net_packet.w_begin		(M_SAVE_GAME);
		net_packet.w_stringZ	("autosave");
		net_packet.w_u8			(0);
		Level().Send			(net_packet,net_flags(TRUE));
		m_save_state			= eSaveStateWaitForUpdate;
		shedule.t_min			= 5000;
		shedule.t_max			= 5000;
		shedule_unregister		();
		shedule_register		();
	}

	if (last_autosave_time() + autosave_interval() >= Device.dwTimeGlobal)
		return;

	if (!ready_for_autosave()) {
		delay_autosave			();
		return;
	}
		
	update_autosave_time		();

	m_save_state				= eSaveStateWaitForText;

	shedule.t_min				= 1;
	shedule.t_max				= 1;
	shedule_unregister			();
	shedule_register			();
}
