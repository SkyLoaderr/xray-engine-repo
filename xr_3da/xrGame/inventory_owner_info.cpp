//////////////////////////////////////////////////////////////////////
// inventory_owner_info.h:	��� ������ � �������� �����������
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryOwner.h"
#include "GameObject.h"
#include "xrMessages.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_registry_container.h"
#include "script_game_object.h"
#include "script_callback.h"
#include "level.h"
#include "script_space.h"

void  CInventoryOwner::OnEvent (NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_INFO_TRANSFER:
		{
			u16				id;
			s32				info_index;
			u8				add_info;

			P.r_u16			(id);				//�����������
			P.r_s32			(info_index);		//����� ���������� ����������
			P.r_u8			(add_info);			//���������� ��� �������� ����������

			if(add_info)
				OnReceiveInfo	((INFO_ID)info_index);
			else
				OnDisableInfo	((INFO_ID)info_index);
		}
		break;
	}
}


class CFindByIDPred
{
public:
	CFindByIDPred(int element_to_find) {element = element_to_find;}
	bool operator () (const INFO_DATA& data) const {return data.id == element;}
private:
	INFO_ID element;
};


void CInventoryOwner::OnReceiveInfo(INFO_ID info_index)
{
	//��������� ���������� callback
	CGameObject* pThisGameObject = dynamic_cast<CGameObject*>(this);
	VERIFY(pThisGameObject);

	SCRIPT_CALLBACK_EXECUTE_2(*m_pInfoCallback, pThisGameObject->lua_game_object(), info_index);

	CInfoPortion info_portion;
	info_portion.Load(info_index);

	//��������� ���������� �������
	info_portion.RunScriptActions(pThisGameObject);

	//�������� �� info portions ������� ����� �������������
	for(u32 i=0; i<info_portion.DisableInfos().size(); i++)
		TransferInfo(info_portion.DisableInfos()[i], false);

	//�������� ������ � ������
	KNOWN_INFO_VECTOR& known_info = known_info_registry.objects();
	KNOWN_INFO_VECTOR_IT it = std::find_if(known_info.begin(), known_info.end(), CFindByIDPred(info_index));
	if( known_info.end() == it)
	{
		known_info.push_back(INFO_DATA(info_index, Level().GetGameTime()));
	}
}

void CInventoryOwner::OnDisableInfo(INFO_ID info_index)
{
	//������� ������ �� �������
	KNOWN_INFO_VECTOR& known_info = known_info_registry.objects();

	KNOWN_INFO_VECTOR_IT it = std::find_if(known_info.begin(), known_info.end(), CFindByIDPred(info_index));
	if( known_info.end() == it)	return;
	known_info.erase(it);
}

void CInventoryOwner::TransferInfo(INFO_ID info_index, bool add_info) const
{
	const CObject* pThisObject = dynamic_cast<const CObject*>(this); VERIFY(pThisObject);

	//���������� �� ������ PDA ����� ���������� � �������
	NET_Packet		P;
	CGameObject::u_EventGen(P, GE_INFO_TRANSFER, pThisObject->ID());
	P.w_u16			(pThisObject->ID());					//�����������
	P.w_s32			(info_index);							//���������
	P.w_u8			(add_info?1:0);							//��������/������� ����������
	CGameObject::u_EventSend(P);
}

bool CInventoryOwner::HasInfo(INFO_ID info_index) const
{
	const KNOWN_INFO_VECTOR* known_info = known_info_registry.objects_ptr ();
	
	if(!known_info) return false;

	if(std::find_if(known_info->begin(), known_info->end(), CFindByIDPred(info_index)) == known_info->end())
		return false;

	return true;
}

