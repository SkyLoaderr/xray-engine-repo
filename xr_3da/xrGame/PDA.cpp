///////////////////////////////////////////////////////////////
// PDA.cpp
// PDA - ������� ��������� � ������
///////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "pda.h"
#include "hudmanager.h"
#include "PhysicsShell.h"
#include "Entity.h"

#include "actor.h"


CPda::CPda(void) 
{
	m_fRadius = 10.0f;

	//����������� ���� ��� PDA
	m_slot = PDA_SLOT;
	//��� � ������, ���� ����� PDA	
	m_ruck = true;

	m_idOriginalOwner = 0xffff;

	m_bPassiveMode = false;
	
	//�������, ������ ����� ����������� � ������� ���������
	TurnOff();

	m_bNewMessage = false;
}

CPda::~CPda(void) 
{
}

BOOL CPda::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

//	CSkeletonAnimated* V = PSkeletonAnimated(Visual());
//	if(V) V->PlayCycle("idle");
	CSkeletonRigid* V = PSkeletonRigid(Visual());
	R_ASSERT(V);


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

//	m_fRadius = 200.f;
		
}

void CPda::net_Destroy() 
{
	//��� �������� PDA � offline 
	//�������������� �������� ��� ������ �� ����
	//�� ������ PDA
	/*for(PDA_LIST_it it = m_PDAList.begin();
					it!=m_PDAList.end(); 
					it++)
	{
		CPda* pPda = (*it);
		pPda->feel_touch_delete(H_Parent());
	}*/




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


	//�������� ������ ���������� PDA
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
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"_new_ PDA detected");
		m_PDAList.push_back(pInvOwner->GetPDA());

		m_NewPDAList.push_back(pInvOwner->GetPDA());
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
			m_DeletedPDAList.push_back(pInvOwner->GetPDA());
		}
		//��� ������ �������� PDA � offline
		else
		{
			for(PDA_LIST_it it = m_PDAList.begin();
					it!=m_PDAList.end(); 
					it++)
			{
				CPda* pPda = (*it);
				if(O == pPda->H_Parent())
				{
					m_PDAList.erase(it);
					m_DeletedPDAList.push_back(pPda);
					break;
				}
			}
		}

	}
}

BOOL CPda::feel_touch_contact(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);
	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(O);

	if(pInvOwner && pEntityAlive->g_Alive() && !pEntityAlive->getDestroy() &&
	   pInvOwner->IsActivePDA() && pInvOwner->GetPDA()!=this &&
	   !pInvOwner->GetPDA()->getDestroy()) 
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

	//������ ��������
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
	
	//���������
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

//�������� ��������� ������� ��������� PDA 
//pda_num - ����� PDA � ����� ������
void CPda::SendMessage(u32 pda_num, EPdaMsg msg, EPdaMsgAnger anger)
{
	//����� PDA � ������ ������� � ������
	u32 i=0;
	for(PDA_LIST_it it = m_PDAList.begin();
		i<=pda_num && it!=m_PDAList.end(); 
		i++, it++){}

	CPda* pPda = (*it);

	if(it == m_PDAList.end()) return;
	if(pPda->IsOff() || pPda->IsPassive()) return;
	
	PdaEventSend(pPda->ID(), msg, anger);
}


//����������� ��������� PDA � ������������ ID
//���� ����� ���� � ������ ���������
void CPda::SendMessageID(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger)
{
	CObject* pObject =  Level().Objects.net_Find(pda_ID);
	CPda* pPda = dynamic_cast<CPda*>(pObject);

	R_ASSERT2(pPda, "Wrong PDA ID");
	if(pPda == NULL) return;

	//���������� ������ PDA � ������ �������� ���������
	PDA_LIST_it it =  std::find(m_PDAList.begin(), m_PDAList.end(), pPda);
	
	//PDA ��� � ������
	if(it == m_PDAList.end()) return;

	PdaEventSend(pda_ID, msg, anger);
}

void CPda::PdaEventSend(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger)
{
	AddMessageToLog(pda_ID, msg, anger, false);

	//������� � ��������� �����
	NET_Packet		P;
	u_EventGen		(P,GE_PDA,pda_ID);
	P.w_u16			(u16(ID()));				//�����������
	P.w_s16			(s16(msg));					//���������
	P.w_s16			(s16(anger));				//������� ��������������
	u_EventSend		(P);
}

//��������� ��������� � ������
void CPda::AddMessageToLog(u32 pda_ID, EPdaMsg msg, EPdaMsgAnger anger, bool receive)
{
	m_bNewMessage = true;

	SPdaMessage pda_message;

	//����� �������
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

			//��������� ��������� ���������, ������ ���� �� ��������
			if(IsActive())
			{
				AddMessageToLog(id, (EPdaMsg)msg, (EPdaMsgAnger)anger, true);
				GetOriginalOwner()->ReceivePdaMessage(id, (EPdaMsg)msg, 
														  (EPdaMsgAnger)anger);
			}
		}
		break;
	case GE_INFO_TRANSFER:
		{
			u16				id;
			s32				info_index;
	
			P.r_u16			(id);				//�����������
			P.r_s32			(info_index);		//����� ���������� ����������

			OnReceiveInfo(info_index);
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

//���� ��������� ��������� ���� ���� � ��� 
//��� ������, �� ���� ������
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


///////////////////////////////////////
// ���������� � �������� ���������� ���
// ������ PDA

//����� �� � ���-��� � �������� �������
bool CPda::IsKnowAbout(int info_index)
{
	KNOWN_INFO_PAIR_IT it = m_mapKnownInfo.find(info_index);

	//��� ��� �������� ��� ����������
	if(it!=m_mapKnownInfo.end()) return true;

	return false;
}

//�������� ���������� ������� PDA
bool CPda::TransferInfoToID(u32 pda_ID, int info_index)
{
	if(!IsKnowAbout(info_index)) return false;

	//������� � ��������� �����
	NET_Packet		P;
	u_EventGen		(P,GE_INFO_TRANSFER,pda_ID);
	P.w_u16			(u16(ID()));				//�����������
	P.w_s32			(info_index);				//���������
	u_EventSend		(P);

	return true;
}

//��������� ����� ������ ����������
void CPda::OnReceiveInfo(int info_index)
{
	KNOWN_INFO_PAIR_IT it = m_mapKnownInfo.find(info_index);

	//��� ��� �������� ��� ����������
	if(it!=m_mapKnownInfo.end()) return;

	//��� �������� FALSE ���� �� ��������....
	//������� ��� ������� ���� � �� �� ���� �����
	m_mapKnownInfo[info_index] = FALSE;

	//���������� ��������� PDA
	GetOriginalOwner()->OnReceiveInfo(info_index);
}

//������� ������ ���� ��������� ��������
void CPda::UpdateQuestions()
{
	m_ActiveQuestionsList.clear();


	for(KNOWN_INFO_PAIR_IT it = m_mapKnownInfo.begin();
		it != m_mapKnownInfo.end(); it++)
	{
		//���������� ������� ���������� � ������� �� ��������
		CInfoPortion info_portion;
		info_portion.Load((*it).first);
		
		for(INFO_QUESTIONS_LIST_it it1 = info_portion.m_QuestionsList.begin();
								   it1 != info_portion.m_QuestionsList.end();
								   it1++)
		{
			//��������� �������� �� ��� ����������� ��� ����������
			//������� �� ����� �������� � ����� �� ������
			SInfoQuestion question = *it1;

			INFO_INDEX_LIST_it it2;
			for(it2 = question.IndexList.begin();
				it2 != question.IndexList.end();
				it2++)
			{
				if(!IsKnowAbout(*it2))
				{
					m_ActiveQuestionsList.push_back(question);
					break;
				}
			}
		}
	}
}