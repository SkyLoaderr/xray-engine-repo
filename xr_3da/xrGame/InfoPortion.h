///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "shared_data.h"

#include "ui/xrXMLParser.h"
#include "map_location.h"
#include "PhraseScript.h"

#include "xml_str_id_loader.h"
#include "encyclopedia_article_defs.h"

#include "GameTaskDefs.h"
#include "PhraseDialogDefs.h"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//������ � ������� ��������, ������� ����� ���� ������������
	//�� ����� InfoPortion
	DIALOG_ID_VECTOR m_DialogNames;

	//������ � ������� ��������, ������� ����� ���� ������������ �������
	//��� ��������� � ����������, ������� ����� ������ InfoPortion
	DIALOG_ID_VECTOR m_ActorDialogNames;

	//������ ������� �� �����
	LOCATIONS_VECTOR	m_MapLocations;
	//������ ������ � ������������, ������� ���������� ���������� 
	ARTICLE_INDEX_VECTOR m_Articles;
	//������ ������ � ������������, ������� ���������� ������������ (�� ��� ������ ����
	//����� �������� ���� ������ ������)
	ARTICLE_INDEX_VECTOR m_ArticlesDisable;
    	
	//�������������� �������
	TASK_INDEX_VECTOR	m_GameTasks;

	//���������� ��������, ������� ������������ ����� ���� ��� 
	//���������� �������� ��������
	CPhraseScript m_PhraseScript;

	//������ � ��������� ��� ������ ����������, �������
	//��������, ����� ��������� ���� info_portion
	DEFINE_VECTOR(INFO_INDEX, INFO_INDEX_VECTOR, INFO_INDEX_VECTOR_IT);
	INFO_INDEX_VECTOR m_DisableInfo;

	//��������� ������������� ����������	
	shared_str		m_text;
	
	//���� true, �� info_portion ����� ��������� 
	//(������� CInventoryOwner::OnReceiveInfo()) ������ ����� ��������� ���������
	//� ������� ����� CInventoryOwner::TransferInfo �������� ��������� CInventoryOwner::OnReceiveInfo
	bool			m_bDeferInit;
};


class CInfoPortion;

//�����  - ������ ����������
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_INDEX, false>,
					 public CXML_IdToIndex<INFO_STR_ID,	INFO_INDEX, CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_INDEX, false> inherited_shared;
	typedef CXML_IdToIndex<INFO_STR_ID, INFO_INDEX, CInfoPortion>	id_to_index;

	friend id_to_index;
public:
				CInfoPortion	(void);
	virtual		~CInfoPortion	(void);

	//������������� info �������
	//���� info � ����� id ������ �� �������������
	//�� ����� �������� �� �����
	virtual void Load	(INFO_STR_ID info_str_id);
	virtual void Load	(INFO_INDEX info_index);

	const LOCATIONS_VECTOR&							MapLocations()	const {return info_data()->m_MapLocations;}
	const ARTICLE_INDEX_VECTOR&						Articles	()	const {return info_data()->m_Articles;}
	const ARTICLE_INDEX_VECTOR&						ArticlesDisable	()	const {return info_data()->m_ArticlesDisable;}
	const TASK_INDEX_VECTOR&						GameTasks	()	const {return info_data()->m_GameTasks;}
	const DIALOG_ID_VECTOR&							DialogNames	()	const {return info_data()->m_DialogNames;}
	const DIALOG_ID_VECTOR&							ActorDialogNames	()	const {return info_data()->m_ActorDialogNames;}
	const SInfoPortionData::INFO_INDEX_VECTOR&		DisableInfos()	const {return info_data()->m_DisableInfo;}
	
	//������ �������������� ���������� �������	
	virtual void RunScriptActions   (const CGameObject* pOwner)	{info_data()->m_PhraseScript.Action(pOwner, NULL, -1);}

	//��������� ������������� ����������
	virtual LPCSTR GetText () const ;
	virtual bool   DeferInit () const;


protected:
    INFO_INDEX	m_InfoIndex;

	void load_shared	(LPCSTR);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	const SInfoPortionData* info_data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};