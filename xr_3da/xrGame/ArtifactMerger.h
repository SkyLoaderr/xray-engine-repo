///////////////////////////////////////////////////////////////
// ArtifactMerger.h
// ArtifactMerger - ���������� ��� ��������� ����� ���������� 
// �� ���������
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory_item.h"

#include "ai_script_space.h"

#include "Artifact.h"
#include "MercuryBall.h"
#include "GraviArtifact.h"
#include "BlackDrops.h"
#include "Needles.h"

DEF_LIST (ARTIFACT_LIST, CArtifact*);

void SetArtifactMergeFunctor(const luabind::functor<void> &artifactMergeFunctor);

class CArtifactMerger : public CInventoryItem
{
typedef	CInventoryItem	inherited;
public:
	CArtifactMerger(void);
	virtual ~CArtifactMerger(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();


	//������� ��� ������ �� ������� ����������
	void AddArtifact(CArtifact* pArtifact);
	void RemoveArtifact(CArtifact* pArtifact);
	void RemoveAllArtifacts();

	//��������� �������� ������� ��� ����
	//����������, ��� ��������� � ������
	//(����� ������������� ���������)
	bool PerformMerge();

	//���������� ��������� ���������� � ������
	template<typename A> int GetArtifactNum()
	{
		int artifact_count = 0;
		for(ARTIFACT_LIST_it it = m_ArtifactList.begin();
							 m_ArtifactList.end() != it;
							 ++it)
							if(dynamic_cast<A*>(*it)) ++artifact_count;
		return artifact_count;
	};

	int GetMercuryBallNum()		{return GetArtifactNum<CMercuryBall>();}
	int GetGraviArtifactNum()	{return GetArtifactNum<CGraviArtifact>();}
	int GetBlackDropsNum()		{return GetArtifactNum<CBlackDrops>();}
	int GetNeedlesNum()			{return GetArtifactNum<CNeedles>();}

	//�������� ������������� ����� ���������� ���������� ���� �� ������
	template<typename A> void DestroyArtifacts(int num_to_destroy)
	{
		int artifact_count=0;
//		for(ARTIFACT_LIST_it it = m_ArtifactList.begin();
//							 (m_ArtifactList.end() != it) &&
//							 (artifact_count<num_to_destroy);)

		ARTIFACT_LIST_it it = m_ArtifactList.begin();
		do
		{
			if(dynamic_cast<A*>(*it)) 
			{
				++artifact_count;
				//������� ������ ���������
				PIItem pIItem = *it;
				pIItem->Drop();
				NET_Packet P;
				CGameObject* pObject = dynamic_cast<CGameObject*>(pIItem->H_Parent());
				R_ASSERT(pObject);
				pObject->u_EventGen(P,GE_DESTROY,pIItem->ID());
		Msg					("ge_destroy: [%d] - %s",pIItem->ID(),*pIItem->cName());
				P.w_u16(u16(pIItem->ID()));
				pObject->u_EventSend(P);
			
				//������� ��������
				m_ArtifactDeletedList.push_back(*it);
				RemoveArtifact(*it);
			
				//������ ����� ������� ������
				it = m_ArtifactList.begin();
			}
			else
			{
				++it;
			}
		} while((m_ArtifactList.end() != it) &&
		  	    (artifact_count<num_to_destroy));
	}

	
	void DestroyMercuryBall(int num_to_destroy)		{DestroyArtifacts<CMercuryBall>(num_to_destroy);}
	void DestroyGraviArtifact(int num_to_destroy)	{DestroyArtifacts<CGraviArtifact>(num_to_destroy);}
	void DestroyBlackDrops(int num_to_destroy)		{DestroyArtifacts<CBlackDrops>(num_to_destroy);}
	void DestroyNeedles(int num_to_destroy)			{DestroyArtifacts<CNeedles>(num_to_destroy);}


	//���������� ������������� ����� ����� ���������� 
	void SpawnArtifact(const char* af_section);
	
	void SpawnMercuryBall()		{SpawnArtifact("af_mercury_ball");}
	void SpawnGraviArtifact()	{SpawnArtifact("af_magnet");}
	void SpawnBlackDrops()		{SpawnArtifact("af_black_drops");}
	void SpawnNeedles()			{SpawnArtifact("af_needles");}


	//������ ���������� ������ ��� ���������
	ARTIFACT_LIST m_ArtifactDeletedList;
	//������ ��� �����������
	ARTIFACT_LIST m_ArtifactNewList;


protected:
	//������ ���������� ���������� � ����������
	ARTIFACT_LIST m_ArtifactList;
};
