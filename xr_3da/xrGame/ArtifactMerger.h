///////////////////////////////////////////////////////////////
// ArtefactMerger.h
// ArtefactMerger - устройство для получения новых артефактов 
// из имеющихся
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory_item.h"
#include "Artifact.h"
#include "MercuryBall.h"
#include "GraviArtifact.h"
#include "BlackDrops.h"
#include "Needles.h"
#include "xrmessages.h"
#include "script_export_space.h"

DEF_LIST (ARTIFACT_LIST, CArtefact*);

// void SetArtefactMergeFunctor(const luabind::functor<void> &artifactMergeFunctor);

class CArtefactMerger : public CInventoryItem
{
typedef	CInventoryItem	inherited;
public:
	CArtefactMerger(void);
	virtual ~CArtefactMerger(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();


	//функции для работы со списком артефактов
	void AddArtefact(CArtefact* pArtefact);
	void RemoveArtefact(CArtefact* pArtefact);
	void RemoveAllArtefacts();

	//выполняет операцию слияния над теми
	//артефктами, что находятся в списке
	//(вызов скриптованной процедуры)
	bool PerformMerge();

	//количество различных артефактов в списке
	template<typename A> int GetArtefactNum()
	{
		int artifact_count = 0;
		for(ARTIFACT_LIST_it it = m_ArtefactList.begin();
							 m_ArtefactList.end() != it;
							 ++it)
							if(dynamic_cast<A*>(*it)) ++artifact_count;
		return artifact_count;
	};

	int GetMercuryBallNum()		{return GetArtefactNum<CMercuryBall>();}
	int GetGraviArtefactNum()	{return GetArtefactNum<CGraviArtefact>();}
	int GetBlackDropsNum()		{return GetArtefactNum<CBlackDrops>();}
	int GetNeedlesNum()			{return GetArtefactNum<CNeedles>();}

	//удаление определенного числа артефактов некоторого типа из списка
	template<typename A> void DestroyArtefacts(int num_to_destroy)
	{
		int artifact_count=0;
//		for(ARTIFACT_LIST_it it = m_ArtefactList.begin();
//							 (m_ArtefactList.end() != it) &&
//							 (artifact_count<num_to_destroy);)

		ARTIFACT_LIST_it it = m_ArtefactList.begin();
		do
		{
			if(dynamic_cast<A*>(*it)) 
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

	
	void DestroyMercuryBall(int num_to_destroy)		{DestroyArtefacts<CMercuryBall>(num_to_destroy);}
	void DestroyGraviArtefact(int num_to_destroy)	{DestroyArtefacts<CGraviArtefact>(num_to_destroy);}
	void DestroyBlackDrops(int num_to_destroy)		{DestroyArtefacts<CBlackDrops>(num_to_destroy);}
	void DestroyNeedles(int num_to_destroy)			{DestroyArtefacts<CNeedles>(num_to_destroy);}


	//добавление определенного числа новых артефактов 
	void SpawnArtefact(const char* af_section);
	
	void SpawnMercuryBall()		{SpawnArtefact("af_mercury_ball");}
	void SpawnGraviArtefact()	{SpawnArtefact("af_magnet");}
	void SpawnBlackDrops()		{SpawnArtefact("af_black_drops");}
	void SpawnNeedles()			{SpawnArtefact("af_needles");}


	//список артефактов только что удаленных
	ARTIFACT_LIST m_ArtefactDeletedList;
	//только что появившихся
	ARTIFACT_LIST m_ArtefactNewList;


protected:
	//список артефактов помещенных в устройство
	ARTIFACT_LIST m_ArtefactList;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CArtefactMerger)
#undef script_type_list
#define script_type_list save_type_list(CArtefactMerger)
