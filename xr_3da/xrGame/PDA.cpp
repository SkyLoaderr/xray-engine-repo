///////////////////////////////////////////////////////////////
// PDA.cpp
// PDA - пейджер сталкеров и солдат
///////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "pda.h"
#include "hudmanager.h"
#include "PhysicsShell.h"

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
	BOOL res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	if (m_pPhysicsShell==0) 
	{
		// Physics (Box)
		Fobb obb; 
		Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); 
		obb.m_rotate.identity();
		
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); 
		R_ASSERT(E); 
		E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); 
		R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}

	setVisible(true);
	setEnabled(true);

	return res;
}

void CPda::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_fRadius = pSettings->r_float(section,"radius");
		
}

void CPda::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	
	inherited::net_Destroy();
}

void CPda::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);

	if(!H_Parent()) return;
	Position().set(H_Parent()->Position());

	
	Fvector	P; 
	P.set(H_Parent()->Position());


	//обновить список дос€гаемых PDA
	if(IsOn())
	{
		m_NewPDAList.clear();
		m_DeletedPDAList.clear();
		feel_touch_update(P,m_fRadius);

		//m_bNewMessage = false;
	}
}

void CPda::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}

void CPda::feel_touch_new(CObject* O) 
{
	if(IsOff()) return;

	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->IsActivePDA() && pInvOwner->GetPDA()!=this) 
	{
		HUD().outMessage(0xffffffff,cName(),"new PDA detected");
		m_PDAList.push_back(pInvOwner->GetPDA());

		m_NewPDAList.push_back(pInvOwner->GetPDA());
	}
}

void CPda::feel_touch_delete(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->IsActivePDA()) 
	{
		HUD().outMessage(0xffffffff,cName(),"a PDA left radius of sight");
		m_PDAList.erase(std::find(m_PDAList.begin(), 
									m_PDAList.end(), 
									pInvOwner->GetPDA()));

		m_DeletedPDAList.push_back(pInvOwner->GetPDA());
	}
}

BOOL CPda::feel_touch_contact(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->IsActivePDA() && pInvOwner->GetPDA()!=this) 
		return TRUE;
	else
		return FALSE;
}


void CPda::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();

	//первый владелец
	if(m_idOriginalOwner == 0xffff)
	{
		CObject* E = dynamic_cast<CObject*>(H_Parent());
		R_ASSERT(E);

		m_idOriginalOwner = E->ID();
		TurnOn();
	}
}

void CPda::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	//выключить
	m_bTurnedOff = true;

	setVisible(true);
	setEnabled(true);
	
	CObject* E = dynamic_cast<CObject*>(H_Parent()); 
	R_ASSERT(E);
	
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f); 
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
		Position().set(XFORM().c);
	}
}


void CPda::renderable_Render() 
{
	if(getVisible() && !H_Parent()) 
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}


CInventoryOwner* CPda::GetOriginalOwner()
{
	CObject* pObject =  Level().Objects.net_Find(GetOriginalOwnerID());
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(pObject);

	return pInvOwner;
}

//отправка сообщени€ другому владельцу PDA 
//pda_num - номер PDA в нашем списке
void CPda::SendMessage(u32 pda_num, EPdaMsg msg, EPdaMsgAnger anger)
{
	//найти PDA с нужным номером в списке
	u32 i=0;
	for(PDA_LIST_it it = m_PDAList.begin();
		i<=pda_num && it!=m_PDAList.end(); 
		i++, it++){}

	CPda* pPda = (*it);

	if(it == m_PDAList.end()) return;
	if(pPda->IsOff() || pPda->IsPassive()) return;
	
	PdaEventSend(pPda->ID(), msg, anger);
}


//отправление сообщению PDA с определенным ID
//если такое есть в списке контактов
void CPda::SendMessageID(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger)
{
	CObject* pObject =  Level().Objects.net_Find(pda_ID);
	CPda* pPda = dynamic_cast<CPda*>(pObject);

	R_ASSERT2(pPda, "Wrong PDA ID");
	if(pPda == NULL) return;

	//определить индекс PDA в списке активных контактов
	PDA_LIST_it it =  std::find(m_PDAList.begin(), m_PDAList.end(), pPda);
	
	//PDA нет в списке
	if(it == m_PDAList.end()) return;

	PdaEventSend(pda_ID, msg, anger);
}

void CPda::PdaEventSend(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger)
{
	AddMessageToLog(pda_ID, msg, anger, false);

	//создать и отправить пакет
	NET_Packet		P;
	u_EventGen		(P,GE_PDA,pda_ID);
	P.w_u16			(u16(ID()));				//отправитель
	P.w_s16			(s16(msg));					//сообщение
	P.w_s16			(s16(anger));				//уровень раздраженности
	u_EventSend		(P);
}

//добавл€ет сообщение в список
void CPda::AddMessageToLog(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger, bool receive)
{
	m_bNewMessage = true;

	SPdaMessage pda_message;

	//новый контакт
	if(m_mapPdaLog.find(pda_ID) == m_mapPdaLog.end())
	{
		m_mapPdaLog[pda_ID].clear();
		pda_message.question = true;
	}
	else
	{
		pda_message.question = !m_mapPdaLog[pda_ID].front().question;
	}


	pda_message.msg = msg;
	pda_message.anger = anger;
	pda_message.receive = receive;
	pda_message.time = Level().GetGameTime();
	
	//if(m_mapPdaLog[pda_ID].size();
	m_mapPdaLog[pda_ID].push_front(pda_message);
}




void CPda::OnEvent(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_PDA:
		{
			u16				id;
			s16				msg;
			s16				anger;

			P.r_u16			(id);
			P.r_s16			(msg);
			P.r_s16			(anger);

			//отправить сообщение владельцу, только если мы включены
			if(IsActive())
			{
				AddMessageToLog(id, (EPdaMsg)msg, (EPdaMsgAnger)anger, true);
				GetOriginalOwner()->ReceivePdaMessage(id, (EPdaMsg)msg, 
														  (EPdaMsgAnger)anger);
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

