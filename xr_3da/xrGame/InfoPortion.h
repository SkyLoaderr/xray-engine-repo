///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "shared_data.h"

#include "ui/xrXMLParser.h"
#include "map_location.h"
#include "PhraseScript.h"
#include "GameTask.h"
#include "xml_str_id_loader.h"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//���������� ������ ������ ����������
	INFO_ID		m_InfoIndex;

	//������ � ������� ��������, ������� ����� ���� ������������
	//�� ����� InfoPortion
	DEFINE_VECTOR(ref_str, DIALOG_NAME_VECTOR, DIALOG_NAME_VECTOR_IT);
	DIALOG_NAME_VECTOR m_DialogNames;

	//������ ������� �� �����
	LOCATIONS_VECTOR	m_MapLocations;
	//������ ������ � ������������, ������� ���������� ���������� 
	ARTICLE_VECTOR		m_Articles;
	
	//�������������� �������
	CGameTask*			m_pGameTask;

	//���������� ��������, ������� ������������ ����� ���� ��� 
	//���������� �������� ��������
	CPhraseScript m_PhraseScript;

	//������ � ��������� ��� ������ ����������, �������
	//��������, ����� ��������� ���� info_portion
	DEFINE_VECTOR(INFO_ID, INFO_INDEX_VECTOR, INFO_INDEX_VECTOR_IT);
	INFO_INDEX_VECTOR m_DisableInfo;

	//��������� ������������� ����������	
	ref_str		m_text;
};


class CInfoPortion;

//�����  - ������ ����������
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_ID>,
					 public CXML_IdToIndex<INFO_STR_ID,	INFO_ID, CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_ID> inherited_shared;
	typedef CXML_IdToIndex<INFO_STR_ID, INFO_ID, CInfoPortion>	id_to_index;

	friend id_to_index;
public:
				CInfoPortion	(void);
	virtual		~CInfoPortion	(void);

	//������������� info �������
	//���� info � ����� id ������ �� �������������
	//�� ����� �������� �� �����
	virtual void Load	(INFO_STR_ID info_str_id);
	virtual void Load	(INFO_ID info_id);

	const LOCATIONS_VECTOR&							MapLocations()	{return info_data()->m_MapLocations;}
	const SInfoPortionData::DIALOG_NAME_VECTOR&		DialogNames()	{return info_data()->m_DialogNames;}
	const SInfoPortionData::INFO_INDEX_VECTOR&		DisableInfos()	{return info_data()->m_DisableInfo;}
	
	//������ �������������� ���������� �������	
	virtual void RunScriptActions   (const CGameObject* pOwner)	{info_data()->m_PhraseScript.Action(pOwner);}

	//��������� ������������� ����������
	virtual LPCSTR GetText ();
	//�������������� �������, ���� ����
	virtual CGameTask* GetTask () {return info_data()->m_pGameTask;}


protected:
    INFO_ID	m_InfoIndex;

	void load_shared	(LPCSTR);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};