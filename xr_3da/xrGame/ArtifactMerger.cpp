///////////////////////////////////////////////////////////////
// ArtifactMerger.cpp
// ArtifactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArtifactMerger.h"
#include "PhysicsShell.h"
//#include "ai_script_processor.h"
#include "xrserver_objects_alife.h"
#include "level.h"

//глобальный указатель на функтор, который
//запускает функцию сочетания артефактов из 
//скиптов
luabind::functor<void> *g_ArtifactMergeFunctor = NULL;

CArtifactMerger::CArtifactMerger(void) 
{
	m_ArtifactList.clear();
}

CArtifactMerger::~CArtifactMerger(void) 
{
	m_ArtifactList.clear();
	xr_delete			(g_ArtifactMergeFunctor);
}

void SetArtifactMergeFunctor(const luabind::functor<void>& artifactMergeFunctor)
{
	g_ArtifactMergeFunctor = xr_new<luabind::functor<void> >(artifactMergeFunctor);
}

BOOL CArtifactMerger::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CArtifactMerger::Load(LPCSTR section) 
{
	//загрузить все возможные комбинации артефактов для получения
	//новых

	inherited::Load(section);
}

void CArtifactMerger::net_Destroy() 
{
	inherited::net_Destroy();
}

void CArtifactMerger::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CArtifactMerger::UpdateCL() 
{
	inherited::UpdateCL();
}


void CArtifactMerger::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CArtifactMerger::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CArtifactMerger::renderable_Render() 
{
	inherited::renderable_Render();
}
////////////////////////////////////////////
//функции для работы со списком артефактов
////////////////////////////////////////////

void CArtifactMerger::AddArtifact(CArtifact* pArtifact)
{
	m_ArtifactList.push_back(pArtifact);
}
void CArtifactMerger::RemoveArtifact(CArtifact* pArtifact)
{
	m_ArtifactList.remove(pArtifact);
}
void CArtifactMerger::RemoveAllArtifacts()
{
	m_ArtifactList.clear();
}

//выполняет операцию слияния над теми
//артефктами, что находятся в списке
//(вызов скриптованной процедуры)
bool CArtifactMerger::PerformMerge()
{
	R_ASSERT2(g_ArtifactMergeFunctor->is_valid(), "The function that perform artifact merge doesn't set yet");
	
	m_ArtifactDeletedList.clear();
	m_ArtifactNewList.clear();
	
	(*g_ArtifactMergeFunctor)(this);
	return false;
}

void CArtifactMerger::SpawnArtifact(const char* af_section)
{
	CSE_Abstract*		D	= F_entity_Create(af_section);
	R_ASSERT		   (D);
	CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = 
								 dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT			(l_tpALifeDynamicObject);
	l_tpALifeDynamicObject->m_tNodeID = this->level_vertex_id();
		
	// Fill
	strcpy				(D->s_name, af_section);
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	u16(this->H_Parent()->ID());
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	Position();
	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);
}