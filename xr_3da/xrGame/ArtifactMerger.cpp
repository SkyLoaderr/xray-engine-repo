///////////////////////////////////////////////////////////////
// ArtefactMerger.cpp
// ArtefactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArtifactMerger.h"
#include "PhysicsShell.h"
//#include "script_process.h"
#include "xrserver_objects_alife.h"
#include "level.h"
#include "script_space.h"

//глобальный указатель на функтор, который
//запускает функцию сочетания артефактов из 
//скиптов
luabind::functor<void> *g_ArtefactMergeFunctor = NULL;

CArtefactMerger::CArtefactMerger(void) 
{
	m_ArtefactList.clear();
}

CArtefactMerger::~CArtefactMerger(void) 
{
	m_ArtefactList.clear();
	xr_delete			(g_ArtefactMergeFunctor);
}

void SetArtefactMergeFunctor(const luabind::functor<void>& artifactMergeFunctor)
{
	g_ArtefactMergeFunctor = xr_new<luabind::functor<void> >(artifactMergeFunctor);
}

BOOL CArtefactMerger::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CArtefactMerger::Load(LPCSTR section) 
{
	//загрузить все возможные комбинации артефактов для получения
	//новых

	inherited::Load(section);
}

void CArtefactMerger::net_Destroy() 
{
	inherited::net_Destroy();
}

void CArtefactMerger::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CArtefactMerger::UpdateCL() 
{
	inherited::UpdateCL();
}


void CArtefactMerger::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CArtefactMerger::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CArtefactMerger::renderable_Render() 
{
	inherited::renderable_Render();
}
////////////////////////////////////////////
//функции для работы со списком артефактов
////////////////////////////////////////////

void CArtefactMerger::AddArtefact(CArtefact* pArtefact)
{
	m_ArtefactList.push_back(pArtefact);
}
void CArtefactMerger::RemoveArtefact(CArtefact* pArtefact)
{
	m_ArtefactList.remove(pArtefact);
}
void CArtefactMerger::RemoveAllArtefacts()
{
	m_ArtefactList.clear();
}

//выполняет операцию слияния над теми
//артефктами, что находятся в списке
//(вызов скриптованной процедуры)
bool CArtefactMerger::PerformMerge()
{
	R_ASSERT2(g_ArtefactMergeFunctor->is_valid(), "The function that perform artifact merge doesn't set yet");
	
	m_ArtefactDeletedList.clear();
	m_ArtefactNewList.clear();
	
	(*g_ArtefactMergeFunctor)(this);
	return false;
}

void CArtefactMerger::SpawnArtefact(const char* af_section)
{
	CSE_Abstract*		D	= F_entity_Create(af_section);
	R_ASSERT		   (D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
								 smart_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT			(l_tpALifeDynamicObject);
	l_tpALifeDynamicObject->m_tNodeID = this->level_vertex_id();
		
	// Fill
	D->s_name			= af_section;
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(this->H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	Position();
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);
}

template<typename A> 
int CArtefactMerger::GetArtefactNum()
{
	int artifact_count = 0;
	for(ARTIFACT_LIST_it it = m_ArtefactList.begin();
							m_ArtefactList.end() != it;
							++it)
						if(smart_cast<A*>(*it)) ++artifact_count;
	return artifact_count;
};

int CArtefactMerger::GetMercuryBallNum()	{return GetArtefactNum<CMercuryBall>();}
int CArtefactMerger::GetGraviArtefactNum()	{return GetArtefactNum<CGraviArtefact>();}
int CArtefactMerger::GetBlackDropsNum()		{return GetArtefactNum<CBlackDrops>();}
int CArtefactMerger::GetNeedlesNum()		{return GetArtefactNum<CNeedles>();}

template<typename A> void CArtefactMerger::DestroyArtefacts(int num_to_destroy)
{
	int artifact_count=0;
//		for(ARTIFACT_LIST_it it = m_ArtefactList.begin();
//							 (m_ArtefactList.end() != it) &&
//							 (artifact_count<num_to_destroy);)

	ARTIFACT_LIST_it it = m_ArtefactList.begin();
	do
	{
		if(smart_cast<A*>(*it)) 
		{
			++artifact_count;
			//удалить объект артефакта
			PIItem pIItem = *it;
			pIItem->Drop();
			NET_Packet P;
			CGameObject* pObject = static_cast<CGameObject*>(pIItem->H_Parent());
			R_ASSERT(pObject);
			pObject->u_EventGen(P,GE_DESTROY,pIItem->ID());
//				Msg		("ge_destroy: [%d] - %s",pIItem->ID(),*pIItem->cName());
			P.w_u16(u16(pIItem->ID()));
			pObject->u_EventSend(P);
		
			//удалить артефакт
			m_ArtefactDeletedList.push_back(*it);
			RemoveArtefact(*it);
		
			//начать поиск сначала списка
			it = m_ArtefactList.begin();
		}
		else
		{
			++it;
		}
	} while((m_ArtefactList.end() != it) &&
		  	(artifact_count<num_to_destroy));
}


void CArtefactMerger::DestroyMercuryBall(int num_to_destroy)		{DestroyArtefacts<CMercuryBall>(num_to_destroy);}
void CArtefactMerger::DestroyGraviArtefact(int num_to_destroy)	{DestroyArtefacts<CGraviArtefact>(num_to_destroy);}
void CArtefactMerger::DestroyBlackDrops(int num_to_destroy)		{DestroyArtefacts<CBlackDrops>(num_to_destroy);}
void CArtefactMerger::DestroyNeedles(int num_to_destroy)			{DestroyArtefacts<CNeedles>(num_to_destroy);}
