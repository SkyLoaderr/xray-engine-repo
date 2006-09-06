///////////////////////////////////////////////////////////////
// ArtefactMerger.h
// ArtefactMerger - ���������� ��� ��������� ����� ���������� 
// �� ���������
///////////////////////////////////////////////////////////////


#pragma once

#include "inventory_item_object.h"
#include "Artifact.h"
#include "MercuryBall.h"
#include "GraviArtifact.h"
#include "BlackDrops.h"
#include "Needles.h"
#include "xrmessages.h"
#include "script_export_space.h"

DEF_LIST (ARTIFACT_LIST, CArtefact*);

// void SetArtefactMergeFunctor(const luabind::functor<void> &artifactMergeFunctor);

class CArtefactMerger : public CInventoryItemObject {
	typedef	CInventoryItemObject	inherited;
public:
	CArtefactMerger(void);
	virtual ~CArtefactMerger(void);

	virtual BOOL net_Spawn			(CSE_Abstract* DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();


	//������� ��� ������ �� ������� ����������
	void AddArtefact(CArtefact* pArtefact);
	void RemoveArtefact(CArtefact* pArtefact);
	void RemoveAllArtefacts();

	//��������� �������� ������� ��� ����
	//����������, ��� ��������� � ������
	//(����� ������������� ���������)
	bool PerformMerge();

	//���������� ��������� ���������� � ������
	template<typename A> int GetArtefactNum();
	int GetMercuryBallNum()		;
	int GetGraviArtefactNum()	;
	int GetBlackDropsNum()		;
	int GetNeedlesNum()			;

	//�������� ������������� ����� ���������� ���������� ���� �� ������
//	template<typename A> void DestroyArtefacts(int num_to_destroy);
//	void DestroyMercuryBall(int num_to_destroy)		;
//	void DestroyGraviArtefact(int num_to_destroy)	;
//	void DestroyBlackDrops(int num_to_destroy)		;
//	void DestroyNeedles(int num_to_destroy)			;


	//���������� ������������� ����� ����� ���������� 
	void SpawnArtefact(const char* af_section);
	
	void SpawnMercuryBall()		{SpawnArtefact("af_mercury_ball");}
	void SpawnGraviArtefact()	{SpawnArtefact("af_magnet");}
	void SpawnBlackDrops()		{SpawnArtefact("af_black_drops");}
	void SpawnNeedles()			{SpawnArtefact("af_needles");}


	//������ ���������� ������ ��� ���������
	ARTIFACT_LIST m_ArtefactDeletedList;
	//������ ��� �����������
	ARTIFACT_LIST m_ArtefactNewList;


protected:
	//������ ���������� ���������� � ����������
	ARTIFACT_LIST m_ArtefactList;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CArtefactMerger)
#undef script_type_list
#define script_type_list save_type_list(CArtefactMerger)
