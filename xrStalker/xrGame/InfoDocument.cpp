///////////////////////////////////////////////////////////////
// InfoDocument.cpp
// InfoDocument - документ, содержащий сюжетную информацию
///////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "InfoDocument.h"
#include "PhysicsShell.h"
#include "PDA.h"
#include "inventoryowner.h"
#include "xrserver_objects_alife_items.h"

CInfoDocument::CInfoDocument(void) 
{
	m_iInfoIndex = -1;
}

CInfoDocument::~CInfoDocument(void) 
{
}


BOOL CInfoDocument::net_Spawn(LPVOID DC) 
{
	BOOL					res = inherited::net_Spawn(DC);

	CSE_Abstract			*l_tpAbstract = static_cast<CSE_Abstract*>(DC);
	CSE_ALifeItemDocument	*l_tpALifeItemDocument = dynamic_cast<CSE_ALifeItemDocument*>(l_tpAbstract);
	R_ASSERT				(l_tpALifeItemDocument);

	m_iInfoIndex			= l_tpALifeItemDocument->m_wDocIndex;

	return					(res);
}

void CInfoDocument::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CInfoDocument::net_Destroy() 
{
	inherited::net_Destroy();
}

void CInfoDocument::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CInfoDocument::UpdateCL() 
{
	inherited::UpdateCL();
}


void CInfoDocument::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
	
	//передать информацию содержащуюся в документе
	//объекту, который поднял документ
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(H_Parent());
	if(!pInvOwner) return;

	//создать и отправить пакет о получении новой информации
	u32 pda_ID		= pInvOwner->GetPDA()->ID();
	
	NET_Packet		P;
	u_EventGen		(P,GE_INFO_TRANSFER,pda_ID);
	P.w_u16			(u16(ID()));				//отправитель
	P.w_s32			(m_iInfoIndex);				//сообщение
	u_EventSend		(P);
}

void CInfoDocument::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CInfoDocument::renderable_Render() 
{
	inherited::renderable_Render();
}