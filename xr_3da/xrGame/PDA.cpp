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

#include "xrserver.h"
#include "xrServer_Objects_ALife_Items.h"
#include "level.h"

#include "specific_character.h"
#include "alife_registry_wrappers.h"
#include "AI/Monsters/BaseMonster/base_monster.h"


CPda::CPda(void)						
{										
	//����������� ���� ��� PDA
	m_slot					= PDA_SLOT;
	//��� � ������, ���� ����� PDA	
	m_flags.set				(Fruck, TRUE);

	m_idOriginalOwner		= 0xffff;
	m_SpecificChracterOwner = NULL;


	m_bPassiveMode = false;
	
	//�������, ������ ����� ����������� � ������� ���������
	TurnOff					();

	m_bNewMessage			= false;
}

CPda::~CPda(void) 
{
}

BOOL CPda::net_Spawn(CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return					(FALSE);
	CSE_Abstract				*abstract = (CSE_Abstract*)(DC);
	CSE_ALifeItemPDA			*pda = smart_cast<CSE_ALifeItemPDA*>(abstract);
	R_ASSERT					(pda);
	m_idOriginalOwner			= pda->m_original_owner;
	m_SpecificChracterOwner		= pda->m_specific_character;

	return						(TRUE);
}

void CPda::net_Destroy() 
{
	inherited::net_Destroy		();
	TurnOff						();
	feel_touch.clear			();
}

void CPda::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_fRadius = pSettings->r_float(section,"radius");
}

void CPda::shedule_Update(u32 dt)	
{
	// Msg							("-SUB-:[%x][%s] CPda::shedule_Update",smart_cast<void*>(this),*cName());
	inherited::shedule_Update	(dt);

	if(!H_Parent()) return;
	Position().set	(H_Parent()->Position());

	//�������� ������ ���������� PDA
	if( IsOn() && Level().CurrentEntity() && Level().CurrentEntity()->ID()==H_Parent()->ID() )
	{
		CEntityAlive* EA = smart_cast<CEntityAlive*>(H_Parent());
		if(!EA || !EA->g_Alive())
		{
			TurnOff();
			return;
		}

//		m_NewPDAList.clear();
//		m_DeletedPDAList.clear();
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

	CInventoryOwner* pNewContactInvOwner	= smart_cast<CInventoryOwner*>(O);
	CInventoryOwner* pOwner					= smart_cast<CInventoryOwner*>( H_Parent() );VERIFY(pOwner);

	pOwner->NewPdaContact					(pNewContactInvOwner);
}

void CPda::feel_touch_delete(CObject* O) 
{
	if(!H_Parent()) return;//feel_touch_delete called from pure_relcase
	CInventoryOwner* pLostContactInvOwner	= smart_cast<CInventoryOwner*>(O);
	CInventoryOwner* pOwner					= smart_cast<CInventoryOwner*>( H_Parent() );VERIFY(pOwner);

	pOwner->LostPdaContact					(pLostContactInvOwner);
}

BOOL CPda::feel_touch_contact(CObject* O) 
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(O);
	if(pInvOwner){
		if( this!=pInvOwner->GetPDA() /*&& 
		 pInvOwner->GetPDA()&& 
		!pInvOwner->GetPDA()->getDestroy()	&& 
		pInvOwner->IsActivePDA()*/)
		{
			CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(O);
			if(pEntityAlive /*&& pEntityAlive->g_Alive()*/ && !pEntityAlive->getDestroy() && !pEntityAlive->cast_base_monster())
				return TRUE;
		}else
		return FALSE;
	}

	return FALSE;
}

void CPda::save(NET_Packet &output_packet)
{
	inherited::save	(output_packet);
	save_data		(m_sFullName, output_packet);
}

void CPda::load(IReader &input_packet)
{
	inherited::load	(input_packet);
	load_data		(m_sFullName, input_packet);
}

void CPda::OnH_A_Chield() 
{
	VERIFY(IsOff());

	//�������� PDA ������ ���� ��� ��������� � ������� ���������
	if(H_Parent()->ID() == m_idOriginalOwner){
		TurnOn					();
		if(m_sFullName.empty()){
			m_sFullName.assign(inherited::Name());
			m_sFullName += " ";
			m_sFullName += (smart_cast<CInventoryOwner*>(H_Parent()))->Name();
		}
	};

	CActor* actor = smart_cast<CActor*>(H_Parent());
	if(!actor) return;



	//transfer all infos from original owner to new owner
	CInfoPortionWrapper	*known_info_registry	= xr_new<CInfoPortionWrapper>();
	known_info_registry->registry().init		(m_idOriginalOwner);
	KNOWN_INFO_VECTOR& known_info				= known_info_registry->registry().objects();

	KNOWN_INFO_VECTOR_IT it = known_info.begin();
	for(int i=0;it!=known_info.end();++it,++i){
		(*it).info_id;	
		NET_Packet		P;
		u_EventGen		(P,GE_INFO_TRANSFER, H_Parent()->ID());
		P.w_u16			(ID());						//�����������
		P.w_stringZ		((*it).info_id);			//���������
		P.w_u8			(1);						//���������� ���������
		u_EventSend		(P);

	}
	known_info.clear	();
	xr_delete			(known_info_registry);

	inherited::OnH_A_Chield		();
}

void CPda::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	//���������
	TurnOff();
}


void CPda::renderable_Render() 
{
	inherited::renderable_Render();
}

CInventoryOwner* CPda::GetOriginalOwner()
{
	CObject* pObject =  Level().Objects.net_Find(GetOriginalOwnerID());
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(pObject);

	return pInvOwner;
}

CObject* CPda::GetOwnerObject()
{
	CObject* pObject =  Level().Objects.net_Find(GetOriginalOwnerID());
	return pObject;
}

//����������� ��������� PDA � ������������ ID
//���� ����� ���� � ������ ���������
void CPda::SendMessageID(u32 pda_ID, EPdaMsg msg, INFO_ID info_id)
{
	CObject* pObject =  Level().Objects.net_Find(pda_ID);
	CPda* pPda = smart_cast<CPda*>(pObject);
	if(!pPda) return;
	CObject* pPdaOwner = pPda->H_Parent();
	xr_vector<CObject*>::iterator it = std::find(feel_touch.begin(), feel_touch.end(), pPdaOwner);
	//���������� ������ PDA � ������ �������� ���������
//	PDA_LIST_it it =  std::find(m_PDAList.begin(), m_PDAList.end(), pPda);
	
	//PDA ��� � ������
	if(it == feel_touch.end()) return;

	PdaEventSend(pda_ID, msg, info_id);
}

void CPda::PdaEventSend(u32 pda_ID, EPdaMsg msg, INFO_ID info_id)
{
	AddMessageToLog(pda_ID, msg, info_id, false);

	//������� � ��������� �����
	NET_Packet		P;
	u_EventGen		(P,GE_PDA,pda_ID);
	P.w_u16			(u16(ID()));				//�����������
	P.w_s16			(s16(msg));					//���������
	P.w_stringZ		(info_id);					//������ ���������� ���� �����
	u_EventSend		(P);
}

//��������� ��������� � ������
void CPda::AddMessageToLog(u32 pda_ID, EPdaMsg msg, INFO_ID info_id, bool receive)
{
	m_bNewMessage = true;

	SPdaMessage pda_message;

	//����� �������
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
	pda_message.info_id = info_id;
	pda_message.receive = receive;
	pda_message.time = Level().GetGameTime();
	
	//if(m_mapPdaLog[pda_ID].size();
	m_mapPdaLog[pda_ID].push_front(pda_message);
}

//�������� ��������� ��������� �� ���� 
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
			shared_str		info_id;

			P.r_u16			(id);
			P.r_s16			(msg);
			P.r_stringZ		(info_id);

			//��������� ��������� ���������, ������ ���� �� ��������
			if(IsActive())
			{
//				GetOriginalOwner()->ReceivePdaMessage(id, (EPdaMsg)msg, info_id);
				AddMessageToLog(id, (EPdaMsg)msg, info_id, true);
			}
		}
		break;
	}

	inherited::OnEvent(P,type);
}


LPCSTR		CPda::Name				()
{
	if( !m_SpecificChracterOwner.size() )
		return inherited::Name();

	if(m_sFullName.empty())
	{
		m_sFullName.assign(inherited::Name());
		
		CSpecificCharacter spec_char;
		spec_char.Load(m_SpecificChracterOwner);
		m_sFullName += " ";
		m_sFullName += xr_string(spec_char.Name());
	}
	
	return m_sFullName.c_str();
}
LPCSTR		CPda::NameComplex		()
{
	return Name();
}

CPda* CPda::GetPdaFromOwner(CObject* owner)
{
	CInventoryOwner* pInvOwner = smart_cast<CInventoryOwner*>(owner);
	return pInvOwner->GetPDA			();
}

void CPda::ActiveContacts(xr_vector<CPda*>& res)
{
	res.clear				();
	xr_vector<CObject*>		v = ActiveContacts();
	xr_vector<CObject*>::iterator it= v.begin();
	for(;it!=v.end();++it)
		res.push_back(GetPdaFromOwner(*it));
}

xr_vector<CObject*>	CPda::ActiveContacts			()							
{
	xr_vector<CObject*> res;

	xr_vector<CObject*>::iterator it= feel_touch.begin();
	for(;it!=feel_touch.end();++it){
		CEntityAlive* pEA = smart_cast<CEntityAlive*>(*it);
		if(!!pEA->g_Alive())
			res.push_back(*it);
	}

	return	res;
}
