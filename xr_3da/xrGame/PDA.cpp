///////////////////////////////////////////////////////////////
// PDA.cpp
// PDA - пейджер сталкеров и солдат
///////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "pda.h"
#include "hudmanager.h"
#include "PhysicsShell.h"
#include "Entity.h"
#include "actor.h"
#include "xrServer_Objects_ALife_Items.h"
#include "level.h"


CPda::CPda(void)						
{										
	m_fRadius = 10.0f;

	//специальный слот дл€ PDA
	m_slot = PDA_SLOT;
	//или в рюкзак, если чужой PDA	
	m_ruck = true;

	m_idOriginalOwner = 0xffff;

	m_bPassiveMode = false;
	
	//включим, только когда присоединим к первому владельцу
	TurnOff();

	m_bNewMessage = false;
}

CPda::~CPda(void) 
{
}

BOOL CPda::net_Spawn(LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);
	CSE_Abstract		*abstract = (CSE_Abstract*)(DC);
	CSE_ALifeItemPDA	*pda = dynamic_cast<CSE_ALifeItemPDA*>(abstract);
	R_ASSERT			(pda);
	m_idOriginalOwner	= pda->m_original_owner;
	
	return				(TRUE);
}
void CPda::net_Destroy() 
{
	inherited::net_Destroy();
	TurnOff				();
}

void CPda::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_fRadius = pSettings->r_float(section,"radius");
}

void CPda::shedule_Update(u32 dt)	
{
	// Msg							("-SUB-:[%x][%s] CPda::shedule_Update",dynamic_cast<void*>(this),*cName());
	inherited::shedule_Update	(dt);

	if(!H_Parent()) return;
	Position().set(H_Parent()->Position());

	//обновить список дос€гаемых PDA
	if(IsOn())
	{
		CEntityAlive* EA = dynamic_cast<CEntityAlive*>(H_Parent());
		if(!EA || !EA->g_Alive())
		{
			TurnOff();
			return;
		}

		m_NewPDAList.clear();
		m_DeletedPDAList.clear();
		feel_touch_update(Position(),m_fRadius);
		//m_bNewMessage = false;
	}
}

void CPda::UpdateCL() 
{
	inherited::UpdateCL();
}

void CPda::feel_touch_new(CObject* O) 
{
	if(IsOff()) return;

	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->IsActivePDA() && this != pInvOwner->GetPDA()) 
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"_new_ PDA detected");
		m_PDAList.push_back(pInvOwner->GetPDA());

		m_NewPDAList.push_back(pInvOwner->GetPDA());
		GetOriginalOwner()->NewPdaContact(pInvOwner);
	}
}

void CPda::feel_touch_delete(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner /*&& /*pInvOwner->IsActivePDA()*/) 
	{

		if(pInvOwner->GetPDA())
		{
			if(bDebug) HUD().outMessage(0xffffffff,cName(),"a PDA left radius of sight");
			m_PDAList.erase(std::find(m_PDAList.begin(), 
										m_PDAList.end(), 
										pInvOwner->GetPDA()));
			if (!O->getDestroy()) {
				m_DeletedPDAList.push_back(pInvOwner->GetPDA());
				GetOriginalOwner()->LostPdaContact(pInvOwner);
			}
		}
		//дл€ случа€ перехода PDA в offline
		else
		{
/*			if(pInvOwner->GetPDA())
			{
				PDA_LIST_it it = std::find(m_PDAList.begin(), m_PDAList.end(), pInvOwner->GetPDA());
				
				if(m_PDAList.end() != it)
				{
					if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"a PDA owner moved to offline");
					m_DeletedPDAList.push_back(pInvOwner->GetPDA());
					m_PDAList.erase(it);
				}
			}
		}

*/
			for(PDA_LIST_it it = m_PDAList.begin();
					m_PDAList.end() != it; 
					++it)
			{
				CPda* pPda = (*it);
				//if(O == pPda->H_Parent())
				if(O->ID() == pPda->GetOriginalOwnerID())
		//		if(!pPda->GetOriginalOwner())
				{
					m_PDAList.erase(it);
					if (!O->getDestroy()) {
						m_DeletedPDAList.push_back(pPda);
					}
					break;
				}
			}
		}
		

	}
}

BOOL CPda::feel_touch_contact(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);
	if(pInvOwner && pInvOwner->GetPDA() && 
		this!=pInvOwner->GetPDA() && 
		!pInvOwner->GetPDA()->getDestroy()	&& 
		pInvOwner->IsActivePDA())
	{
		CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(O);
		if(pEntityAlive && pEntityAlive->g_Alive() && !pEntityAlive->getDestroy())
			return TRUE;
	}
	
	return FALSE;
}


void CPda::OnH_A_Chield() 
{
	VERIFY(IsOff());

	//включить PDA только если оно находитс€ у первого владельца
	if(H_Parent()->ID() == m_idOriginalOwner)
		TurnOn					();

	inherited::OnH_A_Chield		();
}

void CPda::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	//выключить
	TurnOff();
}


void CPda::renderable_Render() 
{
	inherited::renderable_Render();
}

CInventoryOwner* CPda::GetOriginalOwner()
{
	CObject* pObject =  Level().Objects.net_Find(GetOriginalOwnerID());
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(pObject);

	return pInvOwner;
}

CObject* CPda::GetOwnerObject()
{
	CObject* pObject =  Level().Objects.net_Find(GetOriginalOwnerID());
	return pObject;
}

//отправка сообщени€ другому владельцу PDA 
//pda_num - номер PDA в нашем списке
void CPda::SendMessage(u32 pda_num, EPdaMsg msg, INFO_INDEX info_index)
{
	//найти PDA с нужным номером в списке
	u32 i=0;
	for(PDA_LIST_it it = m_PDAList.begin();
		i<=pda_num && m_PDAList.end() != it; 
		++i, ++it){}

	CPda* pPda = (*it);

	if(it == m_PDAList.end()) return;
	if(pPda->IsOff() || pPda->IsPassive()) return;
	
	PdaEventSend(pPda->ID(), msg, info_index);
}


//отправление сообщению PDA с определенным ID
//если такое есть в списке контактов
void CPda::SendMessageID(u32 pda_ID, EPdaMsg msg, INFO_INDEX info_index)
{
	CObject* pObject =  Level().Objects.net_Find(pda_ID);
	CPda* pPda = dynamic_cast<CPda*>(pObject);

	R_ASSERT2(pPda, "Wrong PDA ID");
	if(pPda == NULL) return;

	//определить индекс PDA в списке активных контактов
	PDA_LIST_it it =  std::find(m_PDAList.begin(), m_PDAList.end(), pPda);
	
	//PDA нет в списке
	if(it == m_PDAList.end()) return;

	PdaEventSend(pda_ID, msg, info_index);
}

void CPda::PdaEventSend(u32 pda_ID, EPdaMsg msg, INFO_INDEX info_index)
{
	AddMessageToLog(pda_ID, msg, info_index, false);

	//создать и отправить пакет
	NET_Packet		P;
	u_EventGen		(P,GE_PDA,pda_ID);
	P.w_u16			(u16(ID()));				//отправитель
	P.w_s16			(s16(msg));					//сообщение
	P.w_s32			(info_index);				//индекс информации если нужен
	u_EventSend		(P);
}

//добавл€ет сообщение в список
void CPda::AddMessageToLog(u32 pda_ID, EPdaMsg msg, INFO_INDEX info_index, bool receive)
{
	m_bNewMessage = true;

	SPdaMessage pda_message;

	//новый контакт
	if(m_mapPdaLog.find(pda_ID) == m_mapPdaLog.end())
	{
		m_mapPdaLog[pda_ID].clear();
		
		if(msg == ePdaMsgInfo)
			pda_message.question = false;
		else
			pda_message.question = true;
	}
	else
	{
		pda_message.question = !m_mapPdaLog[pda_ID].front().question;
	}


	pda_message.msg = msg;
	pda_message.info_index = info_index;
	pda_message.receive = receive;
	pda_message.time = Level().GetGameTime();
	
	//if(m_mapPdaLog[pda_ID].size();
	m_mapPdaLog[pda_ID].push_front(pda_message);
}

//получить последнее сообщение из лога 
bool CPda::GetLastMessageFromLog(u32 pda_ID, SPdaMessage& pda_message)
{
	PDA_LOG_PAIR_IT it =  m_mapPdaLog.find(pda_ID);
	if(m_mapPdaLog.end() == it)	return false;
	
	pda_message = (*it).second.front();

	return true;
}



void CPda::OnEvent(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_PDA:
		{
			u16				id;
			s16				msg;
			s32				info_index;

			P.r_u16			(id);
			P.r_s16			(msg);
			P.r_s32			(info_index);

			//отправить сообщение владельцу, только если мы включены
			if(IsActive())
			{
				GetOriginalOwner()->ReceivePdaMessage(id, (EPdaMsg)msg, (INFO_INDEX)info_index);
				AddMessageToLog(id, (EPdaMsg)msg, (INFO_INDEX)info_index, true);
			}
		}
		break;
	}

	inherited::OnEvent(P,type);
}

bool CPda::NeedToAnswer(u32 pda_ID)
{
	if(m_mapPdaLog.find(pda_ID) == m_mapPdaLog.end()) 
		return false;

	if(m_mapPdaLog[pda_ID].front().receive	&&
		m_mapPdaLog[pda_ID].front().question)
		return true;
	else
		return false;
}

//если последнее сообщение было наше и это 
//был вопрос, то ждем ответа
bool CPda::WaitForReply(u32 pda_ID)
{
	if(m_mapPdaLog.find(pda_ID) == m_mapPdaLog.end()) 
		return false;

	if(!m_mapPdaLog[pda_ID].front().receive	&&
		m_mapPdaLog[pda_ID].front().question)
		return true;
	else
		return false;

}